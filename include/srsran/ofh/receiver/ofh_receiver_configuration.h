/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/compression/iq_decompressor.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_decoder.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/receiver/ofh_receiver_timing_parameters.h"
#include "srsran/ofh/receiver/ofh_receiver_warn_unreceived_frames_parameters.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/cyclic_prefix.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver configuration.
struct receiver_config {
  /// Radio sector identifier.
  unsigned sector;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// PRACH eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  /// Uplink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  /// Destination MAC address.
  ether::mac_address mac_dst_address;
  /// Source MAC address.
  ether::mac_address mac_src_address;
  /// Tag control information field.
  std::optional<uint16_t> tci;
  /// Reception window timing parameters.
  rx_window_timing_parameters rx_timing_params;
  /// \brief RU operating bandwidth.
  ///
  /// Set this option when the operating bandwidth of the RU is larger than the configured bandwidth of the cell.
  bs_channel_bandwidth ru_operating_bw;
  /// Uplink compression parameters.
  ru_compression_params ul_compression_params;
  /// PRACH compression parameters.
  ru_compression_params prach_compression_params;
  /// Uplink static compression header flag.
  bool is_uplink_static_compr_hdr_enabled = true;
  /// Enables the Control-Plane PRACH message signalling.
  bool is_prach_control_plane_enabled = false;
  /// Ignore the start symbol value received in the PRACH U-Plane packets.
  bool ignore_prach_start_symbol = false;
  /// If set to true, the payload size encoded in a eCPRI header is ignored.
  bool ignore_ecpri_payload_size_field = false;
  /// If set to true, the sequence id encoded in a eCPRI packet is ignored.
  bool ignore_ecpri_seq_id_field = false;
  /// If set to true, metrics are enabled in the receiver.
  bool are_metrics_enabled = false;
  /// Warn of unreceived Radio Unit frames status.
  warn_unreceived_ru_frames log_unreceived_ru_frames = warn_unreceived_ru_frames::after_traffic_detection;
  /// If set to true, logs late events as warnings, otherwise as info.
  bool enable_log_warnings_for_lates = true;
};

} // namespace ofh
} // namespace srsran
