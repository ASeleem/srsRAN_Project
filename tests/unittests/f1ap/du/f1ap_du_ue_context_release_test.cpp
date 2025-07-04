/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_test_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

class f1ap_du_ue_context_release_test : public f1ap_du_test
{
protected:
  f1ap_du_ue_context_release_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();
    du_ue_index_t ue_index = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX));
    test_ue                = run_f1ap_ue_create(ue_index);
    f1ap_message msg       = test_helpers::generate_ue_context_setup_request(
        gnb_cu_ue_f1ap_id_t{0}, gnb_du_ue_f1ap_id_t{0}, 1, {}, config_helpers::make_default_du_cell_config().nr_cgi);
    run_ue_context_setup_procedure(ue_index, msg);
  }

  void start_procedure(const f1ap_message& msg = generate_ue_context_release_command())
  {
    // Initiate procedure in F1AP.
    f1ap->handle_message(msg);
  }

  ue_test_context* test_ue = nullptr;
};

TEST_F(f1ap_du_ue_context_release_test, when_f1ap_receives_request_then_f1ap_notifies_du_of_ue_context_release)
{
  start_procedure();

  // Let the release timeout expire.
  const unsigned release_timeout = 1000;
  for (unsigned i = 0; i != release_timeout; ++i) {
    this->timer_service.tick();
    this->ctrl_worker.run_pending_tasks();
    if (this->f1ap_du_cfg_handler.last_ue_delete_req.has_value()) {
      break;
    }
  }

  // DU manager receives UE Context Update Release.
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ue_delete_req.has_value());
  const f1ap_ue_delete_request& req = *this->f1ap_du_cfg_handler.last_ue_delete_req;
  ASSERT_EQ(req.ue_index, test_ue->ue_index);
}

TEST_F(f1ap_du_ue_context_release_test,
       when_f1ap_finishes_handling_request_then_f1ap_responds_back_with_ue_context_release_complete)
{
  start_procedure();

  // Let the release timeout expire.
  const unsigned release_timeout = 1000;
  for (unsigned i = 0; i != release_timeout; ++i) {
    this->timer_service.tick();
    this->ctrl_worker.run_pending_tasks();
    if (this->f1ap_du_cfg_handler.last_ue_delete_req.has_value()) {
      break;
    }
  }

  // F1AP sends UE CONTEXT SETUP RESPONSE to CU-CP.
  ASSERT_EQ(this->f1c_gw.last_tx_pdu().pdu.type().value, f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(this->f1c_gw.last_tx_pdu().pdu.successful_outcome().value.type().value,
            f1ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_release_complete);
  const ue_context_release_complete_s& resp =
      this->f1c_gw.last_tx_pdu().pdu.successful_outcome().value.ue_context_release_complete();
  ASSERT_EQ(resp->gnb_du_ue_f1ap_id, (uint64_t)*test_ue->gnb_du_ue_f1ap_id);
  ASSERT_EQ(resp->gnb_cu_ue_f1ap_id, (uint64_t)*test_ue->gnb_cu_ue_f1ap_id);
}

TEST_F(f1ap_du_ue_context_release_test, when_f1ap_receives_request_then_the_rrc_container_is_sent_dl_via_srb)
{
  f1ap_message msg = generate_ue_context_release_command();
  start_procedure(msg);

  // F1AP sends RRC Container present in UE CONTEXT RELEASE COMMAND via SRB1.
  ASSERT_EQ(test_ue->f1c_bearers[1].rx_sdu_notifier.last_pdu,
            msg.pdu.init_msg().value.ue_context_release_cmd()->rrc_container);
}
