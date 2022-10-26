/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "fapi_factory.h"
#include "lib/du_high/du_high.h"
#include "lib/du_high/du_high_executor_strategies.h"
#include "phy_factory.h"
#include "radio_factory.h"
#include "radio_notifier_sample.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/fapi_adaptor/mac/mac_fapi_adaptor_factory.h"
#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"
#include "srsgnb/phy/adapters/phy_error_adapter.h"
#include "srsgnb/phy/adapters/phy_rg_gateway_adapter.h"
#include "srsgnb/phy/adapters/phy_rx_symbol_adapter.h"
#include "srsgnb/phy/adapters/phy_rx_symbol_request_adapter.h"
#include "srsgnb/phy/adapters/phy_timing_adapter.h"
#include "srsgnb/phy/lower/lower_phy_factory.h"
#include "srsgnb/phy/upper/upper_phy_rg_gateway.h"
#include "srsgnb/phy/upper/upper_phy_timing_notifier.h"
#include <atomic>
#include <csignal>
#include <getopt.h>

using namespace srsgnb;
using namespace fapi_adaptor;
using namespace srs_du;

/// \file
/// \brief Example application of a distributed unit (DU) transmitting SIB1 over a radio interface.
///
/// This example runs a DU without an F1 connection to a CU. It integrates the DU high and DU low subcomponents and
/// connects them through the FAPI interface, the DU low is then connected to the lower PHY and a real-time radio
/// interface. The example transmits SIB1 messages.
///
/// The application supports different working profiles, run <tt> du_example -h </tt> for usage details.
/// \cond

namespace {

struct configuration_profile {
  std::string           name;
  std::string           description;
  std::function<void()> function;
};

} // namespace

/// Program parameters.
static const pci_t              pci         = 55;
static nr_band                  band        = nr_band::n3;
static const subcarrier_spacing scs         = subcarrier_spacing::kHz15;
static const subcarrier_spacing scs_common  = subcarrier_spacing::kHz15;
static const cyclic_prefix      cp          = cyclic_prefix::NORMAL;
static const unsigned           nof_ports   = 1;
static const unsigned           nof_sectors = 1;
static unsigned                 dl_arfcn    = 536020;

static const unsigned max_processing_delay_slots  = 4;
static const unsigned max_nof_concurrent_requests = 10;
static const unsigned ul_to_dl_subframe_offset    = 1;

static std::string                               driver_name = "zmq";
static std::string                               device_arguments;
static std::vector<std::string>                  tx_channel_args;
static std::vector<std::string>                  rx_channel_args;
static radio_configuration::over_the_wire_format otw_format = radio_configuration::over_the_wire_format::DEFAULT;
static radio_configuration::clock_sources        clock_src  = {};
static sampling_rate                             srate      = sampling_rate::from_MHz(61.44);
static phy_time_unit                             time_advance_calibration = phy_time_unit::from_seconds(0.0);
static const lower_phy_ta_offset                 ta_offset                = lower_phy_ta_offset::n0;
static const double                              tx_gain                  = 60.0;
static const double                              rx_gain                  = 70.0;

/// From TS38.104 Section 5.3.2 Table 5.3.2-1. Default 20MHz FR1.
static const std::array<uint16_t, NOF_NUMEROLOGIES> nof_prb_dl_grid = {106, 51, 24, 0, 0};
static const std::array<uint16_t, NOF_NUMEROLOGIES> nof_prb_ul_grid = {106, 51, 24, 0, 0};

static unsigned       offset_to_pointA = 40;
static unsigned       K_ssb            = 6;
static const unsigned coreset0_index   = 6;

static std::string           log_level  = "info";
static srslog::basic_logger& du_logger  = srslog::fetch_basic_logger("DU_APP");
static std::atomic<bool>     is_running = {true};

/// PRACH params
static unsigned       prach_msg1_freq_offset    = 0;
static unsigned       prach_root_sequence_index = 0;
static const unsigned zero_correlation_zone     = 0;
static const unsigned prach_config_index        = 16;
static const unsigned num_prach_fd_occasions    = 1;

/// NOTE: ul_center_freq, dl_center_freq and ul_arfcn are derived from dl_arfcn.
static double   dl_center_freq;
static double   ul_center_freq;
static unsigned ul_arfcn;

/// Amplitude control args.
static float baseband_gain_dB       = -2.5F;
static bool  enable_clipping        = false;
static float full_scale_amplitude   = 1.0F;
static float amplitude_ceiling_dBFS = -0.1F;

/// Defines a set of configuration profiles.
static const std::vector<configuration_profile> profiles = {
    {"zmq_20MHz_n7",
     "Single 20MHz FDD in band n7 using ZMQ.",
     []() {
       driver_name              = "zmq";
       device_arguments         = "";
       srate                    = sampling_rate::from_MHz(61.44);
       time_advance_calibration = phy_time_unit::from_seconds(-16.0F / srate.to_Hz());
       dl_arfcn                 = 536020;
       K_ssb                    = 6;
       offset_to_pointA         = 40;
       band                     = nr_band::n7;
       otw_format               = radio_configuration::over_the_wire_format::DEFAULT;
       // Prepare ZMQ addresses using TCP. Transmitter ports start at 5000+channel_id while receivers connect to
       // 6000+channel_id.
       unsigned port_base   = 5000;
       unsigned port_offset = 1000;
       for (unsigned channel_id = 0; channel_id != nof_ports * nof_sectors; ++channel_id) {
         tx_channel_args.emplace_back("tcp://*:" + std::to_string(port_base + channel_id));
         rx_channel_args.emplace_back("tcp://localhost:" + std::to_string(port_base + channel_id + port_offset));
       }
     }},
    {"uhd_20MHz_n7",
     "Single 20MHz FDD in band n7 using UHD.",
     []() {
       driver_name      = "uhd";
       device_arguments = "type=b200";
       srate            = sampling_rate::from_MHz(23.04);
       dl_arfcn         = 536020;
       K_ssb            = 6;
       offset_to_pointA = 40;
       band             = nr_band::n7;
       otw_format       = radio_configuration::over_the_wire_format::SC12;
       clock_src.clock  = radio_configuration::clock_sources::source::INTERNAL;
       clock_src.sync   = radio_configuration::clock_sources::source::INTERNAL;
     }},
    {"zmq_20MHz_n41", "Single 20MHz TDD in band n41 using ZMQ.", []() {
       driver_name      = "zmq";
       device_arguments = "";
       srate            = sampling_rate::from_MHz(61.44);
       dl_arfcn         = 520000;
       K_ssb            = 7;
       offset_to_pointA = 69;
       band             = nr_band::n41;
       otw_format       = radio_configuration::over_the_wire_format::DEFAULT;
       // Prepare ZMQ addresses using TCP. Transmitter ports start at 5000+channel_id while receivers connect to
       // 6000+channel_id.
       unsigned port_base   = 5000;
       unsigned port_offset = 1000;
       for (unsigned channel_id = 0; channel_id != nof_ports * nof_sectors; ++channel_id) {
         tx_channel_args.emplace_back("tcp://*:" + std::to_string(port_base + channel_id));
         rx_channel_args.emplace_back("tcp://localhost:" + std::to_string(port_base + channel_id + port_offset));
       }
     }}};

namespace {

/// This implementation returns back to the F1 interface a fake F1 Setup Response message upon the receival of the F1
/// Setup Request message.
class fake_cucp_handler : public f1c_message_notifier
{
public:
  explicit fake_cucp_handler(f1c_message_handler* handler_ = nullptr) : handler(handler_) {}

  void attach_handler(f1c_message_handler* handler_) { handler = handler_; };

  void on_new_message(const f1c_message& msg) override
  {
    if (msg.pdu.type() != asn1::f1ap::f1_ap_pdu_c::types::init_msg) {
      return;
    }

    f1c_message response;
    if (msg.pdu.init_msg().value.type().value ==
        asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::init_ulrrc_msg_transfer) {
      // Generate a fake DL RRC Message transfer message and pass it back to the DU.
      response.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_DLRRC_MSG_TRANSFER);

      auto& resp                      = response.pdu.init_msg().value.dlrrc_msg_transfer();
      resp->gnb_du_ue_f1_ap_id->value = msg.pdu.init_msg().value.init_ulrrc_msg_transfer()->gnb_du_ue_f1_ap_id->value;
      resp->gnb_cu_ue_f1_ap_id->value = 0;
      resp->srbid->value              = srb_id_to_uint(srb_id_t::srb0);
      static constexpr uint8_t msg4[] = {
          0x20, 0x40, 0x03, 0x82, 0xe0, 0x05, 0x80, 0x08, 0x8b, 0xd7, 0x63, 0x80, 0x83, 0x0f, 0x00, 0x03, 0xe1,
          0x02, 0x04, 0x68, 0x3c, 0x08, 0x01, 0x05, 0x10, 0x48, 0x24, 0x06, 0x54, 0x00, 0x07, 0xc0, 0x00, 0x00,
          0x00, 0x00, 0x04, 0x1b, 0x84, 0x21, 0x00, 0x00, 0x44, 0x0b, 0x28, 0x00, 0x02, 0x41, 0x00, 0x00, 0x10,
          0x34, 0xd0, 0x35, 0x52, 0x4c, 0x40, 0x00, 0x10, 0x01, 0x02, 0x00, 0x02, 0x00, 0x68, 0x04, 0x00, 0x9d,
          0xb2, 0x58, 0xc0, 0xa2, 0x00, 0x72, 0x34, 0x56, 0x78, 0x90, 0x00, 0x00, 0x4b, 0x03, 0x84, 0x10, 0x78,
          0xbb, 0xf0, 0x30, 0x43, 0x80, 0x00, 0x00, 0x07, 0x12, 0x81, 0xc0, 0x00, 0x02, 0x05, 0xef, 0x40, 0x10,
          0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10, 0x0c, 0xa8, 0x18, 0x06, 0x20, 0x00};

      resp->rrc_container.value.resize(sizeof(msg4));
      std::copy(msg4, msg4 + sizeof(msg4), resp->rrc_container.value.begin());
    } else if (msg.pdu.init_msg().value.type().value ==
               asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request) {
      // Generate a fake F1 Setup response message and pass it back to the DU.
      response.pdu.set_successful_outcome();
      response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

      auto& setup_res = response.pdu.successful_outcome().value.f1_setup_resp();
      // Use the same transaction ID as in the request message.
      setup_res->transaction_id.value = msg.pdu.init_msg().value.f1_setup_request()->transaction_id.value;
      setup_res->gnb_cu_name_present  = true;
      setup_res->gnb_cu_name.value.from_string("srsCU");
      setup_res->gnb_cu_rrc_version.value.latest_rrc_version.from_number(2);
    } else {
      return;
    }

    handler->handle_message(response);
  }

private:
  f1c_message_handler* handler = nullptr;
};

/// Dummy implementation of the mac_result_notifier.
class phy_dummy : public mac_result_notifier
{
  mac_cell_result_notifier& cell;

public:
  explicit phy_dummy(mac_cell_result_notifier& cell) : cell(cell) {}

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
};

/// Manages the workers of the app.
struct worker_manager {
  static const uint32_t task_worker_queue_size = 128;

  void stop()
  {
    dl_workers.stop();
    ul_workers.stop();
    ctrl_worker.stop();
    rt_task_worker.stop();
    upper_dl_worker.stop();
    upper_ul_worker.stop();
    lower_prach_worker.stop();
    radio_worker.stop();
  }

  task_worker              ctrl_worker{"Crtl-DU_UL", task_worker_queue_size};
  task_worker              dl_workers{"DU-DL#0", task_worker_queue_size};
  task_worker              ul_workers{"DU-UL#0", task_worker_queue_size};
  task_worker_executor     ctrl_exec{ctrl_worker};
  task_worker_executor     dl_execs{dl_workers};
  task_worker_executor     ul_execs{ul_workers};
  pcell_ul_executor_mapper ul_exec_mapper{&ul_execs};
  cell_dl_executor_mapper  dl_exec_mapper{&dl_execs};
  // Lower PHY RT task executor.
  task_worker          rt_task_worker{"phy_rt_thread", 1, false, os_thread_realtime_priority::MAX_PRIO};
  task_worker_executor rt_task_executor{{rt_task_worker}};
  // PRACH lower PHY executor
  task_worker          lower_prach_worker{"Lower PHY PRACH worker", task_worker_queue_size};
  task_worker_executor lower_prach_executor{lower_prach_worker};
  // Upper phy task executor
  task_worker          upper_dl_worker{"PHY DL worker", task_worker_queue_size};
  task_worker_executor upper_dl_executor{upper_dl_worker};
  task_worker          upper_ul_worker{"PHY UL worker", task_worker_queue_size};
  task_worker_executor upper_ul_executor{upper_ul_worker};
  // Radio task executor
  task_worker          radio_worker{"Radio worker", task_worker_queue_size};
  task_worker_executor radio_executor{radio_worker};
};

} // namespace

static lower_phy_configuration create_lower_phy_configuration(baseband_gateway*             bb_gateway,
                                                              lower_phy_rx_symbol_notifier* rx_symbol_notifier,
                                                              lower_phy_timing_notifier*    timing_notifier,
                                                              lower_phy_error_notifier*     error_notifier,
                                                              task_executor&                prach_executor)
{
  float tx_scale = 1.0F / std::sqrt(NRE * nof_prb_dl_grid[to_numerology_value(scs)]);

  lower_phy_configuration phy_config;
  phy_config.srate                      = srate;
  phy_config.scs                        = scs;
  phy_config.max_processing_delay_slots = max_processing_delay_slots;
  phy_config.ul_to_dl_subframe_offset   = ul_to_dl_subframe_offset;
  phy_config.time_advance_calibration   = time_advance_calibration;
  phy_config.ta_offset                  = ta_offset;
  phy_config.tx_scale                   = tx_scale;
  phy_config.cp                         = cp;
  phy_config.bb_gateway                 = bb_gateway;
  phy_config.error_notifier             = error_notifier;
  phy_config.rx_symbol_notifier         = rx_symbol_notifier;
  phy_config.timing_notifier            = timing_notifier;
  phy_config.prach_async_executor       = &prach_executor;

  // Amplitude controller configuration.
  phy_config.amplitude_config.full_scale_lin  = full_scale_amplitude;
  phy_config.amplitude_config.ceiling_dBFS    = amplitude_ceiling_dBFS;
  phy_config.amplitude_config.enable_clipping = enable_clipping;
  phy_config.amplitude_config.input_gain_dB   = baseband_gain_dB;

  for (unsigned sector_id = 0; sector_id != nof_sectors; ++sector_id) {
    lower_phy_sector_description sector_config;
    sector_config.bandwidth_rb = nof_prb_dl_grid[to_numerology_value(scs)];
    sector_config.dl_freq_hz   = dl_center_freq;
    sector_config.ul_freq_hz   = ul_center_freq;
    for (unsigned port_id = 0; port_id != nof_ports; ++port_id) {
      lower_phy_sector_port_mapping port_mapping;
      port_mapping.stream_id  = sector_id;
      port_mapping.channel_id = port_id;
      sector_config.port_mapping.push_back(port_mapping);
    }
    phy_config.sectors.push_back(sector_config);
    phy_config.nof_channels_per_stream.push_back(nof_ports);
  }
  phy_config.log_level = log_level;

  return phy_config;
}

static void signal_handler(int sig)
{
  fmt::print("Received signal {}\n", sig);
  is_running = false;
}

static void usage(std::string prog)
{
  fmt::print("Usage: {} [-P profile] [-D duration] [-v level] \n", prog);
  fmt::print("\t-P Profile. [Default {}]\n", profiles.front().name);
  for (const configuration_profile& profile : profiles) {
    fmt::print("\t\t {:<30}{}\n", profile.name, profile.description);
  }
  fmt::print("\t-v Logging level. [Default {}]\n", log_level);
  fmt::print("\t-c Enable amplitude clipping. [Default {}]\n", enable_clipping);
  fmt::print("\t-b Baseband gain prior to clipping (in dB). [Default {}]\n", baseband_gain_dB);
  fmt::print("\t-h print this message.\n");
}

static int parse_args(int argc, char** argv)
{
  std::string profile_name;

  int opt = 0;
  while ((opt = ::getopt(argc, argv, "D:P:L:v:b:ch")) != -1) {
    switch (opt) {
      case 'P':
        if (optarg != nullptr) {
          profile_name = std::string(optarg);
        }
        break;
      case 'v':
        log_level = std::string(optarg);
        break;
      case 'c':
        enable_clipping = true;
        break;
      case 'b':
        if (optarg != nullptr) {
          baseband_gain_dB = std::strtof(optarg, nullptr);
        }
        break;
      case 'h':
      default:
        usage(argv[0]);
        return -1;
    }
  }

  // Search profile if set.
  if (!profile_name.empty()) {
    bool found = false;
    for (const auto& profile : profiles) {
      if (profile_name == profile.name) {
        profile.function();
        du_logger.info("Loading profile: '{}'", profile.name);
        found = true;
        break;
      }
    }
    if (!found) {
      usage(argv[0]);
      du_logger.error("Invalid profile: '{}'", profile_name);
      return -1;
    }
  } else {
    const configuration_profile& default_profile = profiles.front();
    du_logger.info("Loading '{}' as the default profile", default_profile.name);
    default_profile.function();
  }

  return 0;
}

static fapi::prach_config generate_prach_config_tlv()
{
  fapi::prach_config config = {};

  config.prach_res_config_index = 0;
  config.prach_sequence_length  = fapi::prach_sequence_length_type::long_sequence;
  config.prach_scs              = prach_subcarrier_spacing::values::kHz1_25;
  config.prach_ul_bwp_pusch_scs = scs;
  config.restricted_set         = restricted_set_config::UNRESTRICTED;
  config.num_prach_fd_occasions = num_prach_fd_occasions;
  config.prach_config_index     = prach_config_index;
  config.prach_format           = fapi::prach_format_type::zero;
  config.num_prach_td_occasions = 1;
  config.num_preambles          = 1;
  config.start_preamble_index   = 0;

  // Add FD occasion info.
  config.fd_occasions.emplace_back();
  fapi::prach_fd_occasion_config& fd_occasion = config.fd_occasions.back();
  fd_occasion.prach_root_sequence_index       = prach_root_sequence_index;
  fd_occasion.prach_freq_offset               = prach_msg1_freq_offset;
  fd_occasion.prach_zero_corr_conf            = zero_correlation_zone;

  return config;
}

static fapi::carrier_config generate_carrier_config_tlv()
{
  fapi::carrier_config config = {};

  // NOTE; for now we only need to fill the nof_prb_ul_grid.
  config.ul_grid_size = nof_prb_ul_grid;

  return config;
}

static std::unique_ptr<radio_session> build_radio(task_executor& executor, radio_notification_handler& radio_handler)
{
  radio_params params;
  params.device_args     = device_arguments;
  params.log_level       = log_level;
  params.srate           = srate;
  params.otw_format      = otw_format;
  params.nof_sectors     = nof_sectors;
  params.nof_ports       = nof_ports;
  params.dl_frequency_hz = dl_center_freq;
  params.tx_gain         = tx_gain;
  params.tx_channel_args = tx_channel_args;
  params.ul_frequency_hz = ul_center_freq;
  params.rx_gain         = rx_gain;
  params.rx_channel_args = rx_channel_args;

  return create_radio(driver_name, params, executor, radio_handler);
}

static void fill_cell_prach_cfg(du_cell_config& cell_cfg)
{
  cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index = prach_config_index;
  cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_fdm = num_prach_fd_occasions;
  cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_frequency_start =
      prach_msg1_freq_offset;
  cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.zero_correlation_zone_config =
      zero_correlation_zone;
  cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().prach_root_seq_index = prach_root_sequence_index;
}

int main(int argc, char** argv)
{
  srslog::init();
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);
  du_logger.set_level(srslog::basic_levels::info);

  // Parse arguments.
  int ret = parse_args(argc, argv);
  if (ret < 0) {
    return ret;
  }

  worker_manager workers;

  // Calculate derived frequency parameters.
  dl_center_freq = band_helper::nr_arfcn_to_freq(dl_arfcn);
  ul_arfcn       = band_helper::get_ul_arfcn_from_dl_arfcn(dl_arfcn);
  ul_center_freq = band_helper::nr_arfcn_to_freq(ul_arfcn);
  du_logger.info("Starting du_example with DL_ARFCN={}, UL_ARFCN={}, DL center frequency {} Hz, UL center frequency {} "
                 "Hz, tx_gain={} dB, rx_gain={} dB",
                 dl_arfcn,
                 ul_arfcn,
                 dl_center_freq,
                 ul_center_freq,
                 tx_gain,
                 rx_gain);

  // Create radio.
  radio_notification_handler_printer radio_event_printer;
  auto                               radio = build_radio(workers.radio_executor, radio_event_printer);
  report_fatal_error_if_not(radio, "Unable to create radio session.");
  du_logger.info("Radio driver '{}' created successfully", driver_name);

  // Create lower and upper PHY adapters.
  phy_error_adapter             phy_err_printer(log_level);
  phy_rx_symbol_adapter         phy_rx_adapter;
  phy_rg_gateway_adapter        rg_gateway_adapter;
  phy_timing_adapter            phy_time_adapter;
  phy_rx_symbol_request_adapter phy_rx_symbol_req_adapter;

  // Create lower PHY.
  lower_phy_configuration lower_phy_config = create_lower_phy_configuration(&radio->get_baseband_gateway(),
                                                                            &phy_rx_adapter,
                                                                            &phy_time_adapter,
                                                                            &phy_err_printer,
                                                                            workers.lower_prach_executor);
  auto                    lower            = create_lower_phy(lower_phy_config, max_nof_concurrent_requests);
  report_fatal_error_if_not(lower, "Unable to create lower PHY.");
  du_logger.info("Lower PHY created successfully");

  // Create upper PHY.
  upper_phy_params upper_params;
  upper_params.log_level         = srslog::str_to_basic_level(log_level);
  upper_params.nof_ports         = nof_ports;
  upper_params.dl_pipeline_depth = max_processing_delay_slots;
  upper_params.dl_bw_rb          = nof_prb_dl_grid[to_numerology_value(scs)];
  upper_params.ul_bw_rb          = nof_prb_ul_grid[to_numerology_value(scs)];

  auto upper = create_upper_phy(upper_params,
                                &rg_gateway_adapter,
                                &workers.upper_dl_executor,
                                &workers.upper_ul_executor,
                                &phy_rx_symbol_req_adapter);
  report_fatal_error_if_not(upper, "Unable to create upper PHY.");
  du_logger.info("Upper PHY created successfully");

  // Make connections between upper and lower PHYs.
  phy_rx_adapter.connect(&upper->get_rx_symbol_handler());
  phy_time_adapter.connect(&upper->get_timing_handler());
  rg_gateway_adapter.connect(&lower->get_rg_handler());
  phy_rx_symbol_req_adapter.connect(&lower->get_request_handler());

  // Create FAPI adaptors.
  const unsigned sector_id   = 0;
  auto           phy_adaptor = build_phy_fapi_adaptor(sector_id,
                                            scs,
                                            scs_common,
                                            upper->get_downlink_processor_pool(),
                                            upper->get_downlink_resource_grid_pool(),
                                            upper->get_uplink_request_processor(),
                                            upper->get_uplink_resource_grid_pool(),
                                            upper->get_uplink_slot_pdu_repository(),
                                            generate_prach_config_tlv(),
                                            generate_carrier_config_tlv());
  report_fatal_error_if_not(phy_adaptor, "Unable to create PHY adaptor.");
  upper->set_results_notifier(phy_adaptor->get_rx_results_notifier());
  auto mac_adaptor = build_mac_fapi_adaptor(0, scs, phy_adaptor->get_slot_message_gateway());
  report_fatal_error_if_not(mac_adaptor, "Unable to create MAC adaptor.");
  phy_adaptor->set_slot_time_message_notifier(mac_adaptor->get_slot_time_notifier());
  phy_adaptor->set_slot_data_message_notifier(mac_adaptor->get_slot_data_notifier());
  upper->set_timing_notifier(phy_adaptor->get_timing_notifier());
  du_logger.info("FAPI adaptors created successfully");

  // Cell configuration.
  struct du_cell_config_master_params cell_config;
  cell_config.pci               = pci;
  cell_config.scs_common        = scs_common;
  cell_config.nof_crbs          = nof_prb_dl_grid[to_numerology_value(scs)];
  cell_config.dl_arfcn          = dl_arfcn;
  cell_config.band              = band;
  cell_config.offset_to_point_a = offset_to_pointA;
  cell_config.coreset0_index    = coreset0_index;

  fake_cucp_handler f1c_notifier;
  phy_dummy         phy(mac_adaptor->get_cell_result_notifier());

  du_high_configuration du_hi_cfg = {};
  du_hi_cfg.du_mng_executor       = &workers.ctrl_exec;
  du_hi_cfg.ul_executors          = &workers.ul_exec_mapper;
  du_hi_cfg.dl_executors          = &workers.dl_exec_mapper;
  du_hi_cfg.f1c_notifier          = &f1c_notifier;
  du_hi_cfg.phy_adapter           = &phy;
  du_hi_cfg.cells                 = {config_helpers::make_default_du_cell_config(cell_config)};

  du_cell_config& cell_cfg = du_hi_cfg.cells.front();
  cell_cfg.ssb_cfg.k_ssb   = K_ssb;

  // Fill cell specific PRACH configuration.
  fill_cell_prach_cfg(cell_cfg);

  du_high du_obj(du_hi_cfg);
  f1c_notifier.attach_handler(&du_obj.get_f1c_message_handler());
  du_logger.info("DU-High created successfully");

  // Set signal handler.
  ::signal(SIGINT, signal_handler);
  ::signal(SIGTERM, signal_handler);
  ::signal(SIGHUP, signal_handler);
  ::signal(SIGQUIT, signal_handler);
  ::signal(SIGKILL, signal_handler);

  // Start execution.
  du_logger.info("Starting DU-High...");
  du_obj.start();
  du_logger.info("DU-High started successfully");

  // Give some time to the MAC to start.
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // Configure the DU slot handler.
  du_cell_index_t cell_id = to_du_cell_index(0);
  mac_adaptor->set_cell_slot_handler(du_obj.get_slot_handler(cell_id));
  mac_adaptor->set_cell_rach_handler(du_obj.get_rach_handler(cell_id));
  mac_adaptor->set_cell_pdu_handler(du_obj.get_pdu_handler(cell_id));
  mac_adaptor->set_cell_crc_handler(du_obj.get_control_information_handler(cell_id));

  // Start processing.
  du_logger.info("Starting lower PHY...");
  lower->get_controller().start(workers.rt_task_executor);
  du_logger.info("Lower PHY started successfully");

  while (is_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  du_logger.info("Stopping lower PHY...");
  lower->get_controller().stop();
  du_logger.info("Lower PHY stopped successfully");

  du_logger.info("Stopping executors...");
  workers.stop();
  du_logger.info("Executors stopped successfully");

  srslog::flush();

  return 0;
}

/// \endcond