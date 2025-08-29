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

#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class si_message_scheduler
{
public:
  si_message_scheduler(const cell_configuration&                  cfg_,
                       pdcch_resource_allocator&                  pdcch_sch,
                       const std::optional<si_scheduling_config>& si_sched_cfg_);

  /// \brief Performs broadcast SI message scheduling.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  void run_slot(cell_slot_resource_allocator& res_grid);

  /// \brief Update the SI messages.
  void handle_si_message_update_indication(unsigned                                   version,
                                           const std::optional<si_scheduling_config>& new_si_sched_cfg);

  /// Called when cell is deactivated.
  void stop();

private:
  struct message_window_context {
    /// SI message window.
    interval<slot_point> window;
    /// Number of SI message transmissions within the current window.
    unsigned nof_tx_in_current_window = 0;
    /// Total number of SI message transmissions.
    unsigned long total_nof_tx = 0;
  };

  void update_si_message_windows(slot_point sl_tx);

  void schedule_pending_si_messages(cell_slot_resource_allocator& res_grid);

  bool allocate_si_message(unsigned si_message, cell_slot_resource_allocator& res_grid);

  void fill_si_grant(cell_slot_resource_allocator& res_grid,
                     unsigned                      si_message,
                     crb_interval                  si_crbs_grant,
                     uint8_t                       time_resource,
                     const dmrs_information&       dmrs_info,
                     unsigned                      tbs,
                     const message_window_context& message_context);

  // Configuration of the broadcast SI messages.
  const scheduler_si_expert_config&   expert_cfg;
  const cell_configuration&           cell_cfg;
  pdcch_resource_allocator&           pdcch_sch;
  std::optional<si_scheduling_config> si_sched_cfg;
  srslog::basic_logger&               logger;

  std::vector<message_window_context> pending_messages;
  unsigned                            version = 0;
};

} // namespace srsran
