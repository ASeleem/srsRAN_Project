/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_low_appconfig_translators.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "du_low_appconfig.h"

using namespace srsran;
using namespace std::chrono_literals;

void srsran::fill_du_low_worker_manager_config(worker_manager_config& config, const du_low_appconfig& unit_cfg)
{
  config.nof_low_prio_threads     = unit_cfg.expert_execution_cfg.threads.non_rt_threads.nof_non_rt_threads;
  config.low_prio_task_queue_size = unit_cfg.expert_execution_cfg.threads.non_rt_threads.non_rt_task_queue_size;
  config.low_prio_sched_config    = unit_cfg.expert_execution_cfg.affinities.low_priority_cpu_cfg;
}
