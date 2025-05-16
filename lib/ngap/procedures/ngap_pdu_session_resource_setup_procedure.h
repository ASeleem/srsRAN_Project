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

#include "../ngap_validators/ngap_validators.h"
#include "../ue_context/ngap_ue_context.h"
#include "metrics/ngap_metrics_aggregator.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_pdu_session_resource_setup_procedure
{
public:
  ngap_pdu_session_resource_setup_procedure(const cu_cp_pdu_session_resource_setup_request&    request_,
                                            const asn1::ngap::pdu_session_res_setup_request_s& asn1_request_,
                                            const ngap_ue_ids&                                 ue_ids_,
                                            ngap_cu_cp_notifier&                               cu_cp_notifier_,
                                            ngap_metrics_aggregator&                           metrics_handler_,
                                            ngap_message_notifier&                             amf_notif_,
                                            ngap_ue_logger&                                    logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "PDU Session Resource Setup Procedure"; }

private:
  void combine_pdu_session_resource_setup_response();

  // Result senders.
  bool send_pdu_session_resource_setup_response();

  const cu_cp_pdu_session_resource_setup_request    request;
  const asn1::ngap::pdu_session_res_setup_request_s asn1_request;
  cu_cp_pdu_session_resource_setup_response         validation_response;
  byte_buffer                                       nas_pdu;
  const ngap_ue_ids                                 ue_ids;
  cu_cp_pdu_session_resource_setup_response         response;
  ngap_cu_cp_notifier&                              cu_cp_notifier;
  ngap_metrics_aggregator&                          metrics_handler;
  ngap_message_notifier&                            amf_notifier;
  ngap_ue_logger&                                   logger;

  cu_cp_ue_context_release_request ue_context_release_request;

  // Maps PDU session ID to S-NSSAI for metrics.
  std::map<pdu_session_id_t, s_nssai_t> pdu_session_id_to_snssai;

  // Procedure outcomes.
  pdu_session_resource_setup_validation_outcome verification_outcome;
};

} // namespace srs_cu_cp
} // namespace srsran
