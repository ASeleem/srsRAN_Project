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

#include "../du_cell_manager.h"
#include "../du_ue/du_ue_manager.h"
#include "procedure_logger.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

/// \brief Procedure used to interrupt all the DU activity.
class du_stop_procedure
{
public:
  explicit du_stop_procedure(du_ue_manager&           ue_mng,
                             du_cell_manager&         cell_mng_,
                             f1ap_connection_manager& f1ap_conn_mng_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<void> stop_ue_traffic();

  du_ue_manager&           ue_mng;
  du_cell_manager&         cell_mng;
  f1ap_connection_manager& f1ap_conn_mng;

  du_procedure_logger proc_logger;

  std::vector<du_ue_index_t> ues_to_stop;
};

} // namespace srs_du
} // namespace srsran
