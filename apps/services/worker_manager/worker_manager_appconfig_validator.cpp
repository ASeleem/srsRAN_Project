/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "worker_manager_appconfig_validator.h"
#include "worker_manager_appconfig.h"

using namespace srsran;

bool srsran::validate_expert_execution_appconfig(const expert_execution_appconfig& config)
{
  // Ensure the number of non-real time threads does not exceed the number of CPU cores
  auto&    cpu_desc  = cpu_architecture_info::get();
  uint32_t nof_cores = cpu_desc.get_host_nof_available_cpus();
  if (config.threads.main_pool.nof_threads and config.threads.main_pool.nof_threads.value() > nof_cores) {
    fmt::print("Invalid expert execution config: main_pool.nof_threads={} must not exceed nof_cores={}\n",
               config.threads.main_pool.nof_threads.value(),
               nof_cores);
    return false;
  }

  return true;
}
