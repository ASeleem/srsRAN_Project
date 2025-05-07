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

#include "../../cu_cp_impl_interface.h"
#include "../../mobility_manager/mobility_manager_impl.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the handover of a UE between two different cells managed by the same CU.
class intra_cu_handover_target_routine
{
public:
  intra_cu_handover_target_routine(const cu_cp_intra_cu_handover_target_request& request_,
                                   cu_cp_ue_context_release_handler&             ue_context_release_handler_,
                                   cu_cp_ue_context_manipulation_handler&        cu_cp_handler_,
                                   ue_manager&                                   ue_mng_,
                                   mobility_manager&                             mobility_mng_,
                                   srslog::basic_logger&                         logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Intra CU Handover Target Routine"; }

private:
  up_config_update get_next_config_from_radio_bearer_cfg();

  const cu_cp_intra_cu_handover_target_request request;

  // Pointer to UE in the source DU.
  cu_cp_ue* source_ue = nullptr;
  // Pointer to UE in the target DU.
  cu_cp_ue* target_ue = nullptr;

  cu_cp_ue_context_release_handler&      ue_context_release_handler;
  cu_cp_ue_context_manipulation_handler& cu_cp_handler;
  ue_manager&                            ue_mng;
  mobility_manager&                      mobility_mng;

  srslog::basic_logger& logger;

  // (Sub-)routine requests.
  rrc_reconfiguration_procedure_request rrc_reconfig_args;
  cu_cp_ue_context_release_command      ue_context_release_command;

  // (Sub-)routine results.
  bool reconf_result = false;
};

} // namespace srs_cu_cp
} // namespace srsran
