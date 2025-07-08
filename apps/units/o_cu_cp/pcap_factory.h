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

#include "apps/services/worker_manager/pcap_executor_mapper.h"
#include "apps/units/o_cu_cp/o_cu_cp_unit_config.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/signal_observer.h"

namespace srsran {

struct o_cu_cp_dlt_pcaps {
  std::unique_ptr<dlt_pcap> ngap;
  std::unique_ptr<dlt_pcap> f1ap;
  std::unique_ptr<dlt_pcap> e1ap;
  std::unique_ptr<dlt_pcap> e2ap;

  std::unique_ptr<signal_observer> ngap_sig_handler;
  std::unique_ptr<signal_observer> f1ap_sig_handler;
  std::unique_ptr<signal_observer> e1ap_sig_handler;
  std::unique_ptr<signal_observer> e2ap_sig_handler;

  /// \brief Close (and flush) the PCAPs without destroying the objects.
  void close()
  {
    if (ngap) {
      ngap->close();
    }
    if (f1ap) {
      f1ap->close();
    }
    if (e1ap) {
      e1ap->close();
    }
    if (e2ap) {
      e2ap->close();
    }
  }

  /// \brief Destroy (close and flush) the PCAPs.
  void reset()
  {
    ngap_sig_handler.reset();
    f1ap_sig_handler.reset();
    e1ap_sig_handler.reset();
    e2ap_sig_handler.reset();

    ngap.reset();
    f1ap.reset();
    e1ap.reset();
    e2ap.reset();
  }
};

/// Creates the DLT PCAPs of the O-RAN CU-CP.
inline o_cu_cp_dlt_pcaps create_o_cu_cp_dlt_pcap(const o_cu_cp_unit_config& config,
                                                 pcap_executor_mapper&      exec_mapper,
                                                 signal_dispatcher&         signal_source)
{
  o_cu_cp_dlt_pcaps             pcaps;
  const cu_cp_unit_pcap_config& pcap_cfg = config.cucp_cfg.pcap_cfg;
  pcaps.ngap = pcap_cfg.ngap.enabled ? create_ngap_pcap(pcap_cfg.ngap.filename, exec_mapper.get_ngap_executor())
                                     : create_null_dlt_pcap();
  pcaps.ngap_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.ngap->flush(); });

  pcaps.f1ap = pcap_cfg.f1ap.enabled ? create_f1ap_pcap(pcap_cfg.f1ap.filename, exec_mapper.get_f1ap_executor())
                                     : create_null_dlt_pcap();
  pcaps.f1ap_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.f1ap->flush(); });

  pcaps.e1ap = pcap_cfg.e1ap.enabled ? create_e1ap_pcap(pcap_cfg.e1ap.filename, exec_mapper.get_e1ap_executor())
                                     : create_null_dlt_pcap();
  pcaps.e1ap_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.e1ap->flush(); });

  pcaps.e2ap             = config.e2_cfg.pcaps.enabled
                               ? create_e2ap_pcap(config.e2_cfg.pcaps.filename, exec_mapper.get_e2ap_executor())
                               : create_null_dlt_pcap();
  pcaps.e2ap_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.e2ap->flush(); });

  return pcaps;
}

} // namespace srsran
