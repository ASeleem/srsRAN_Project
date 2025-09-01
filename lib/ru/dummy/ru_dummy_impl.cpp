/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_dummy_impl.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ru/dummy/ru_dummy_configuration.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include <chrono>
#include <cstdint>
#include <thread>

using namespace srsran;

static inline uint64_t get_current_system_slot(std::chrono::microseconds slot_duration,
                                               uint64_t                  nof_slots_per_hyper_system_frame)
{
  // Get the time since the epoch.
  auto time_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::high_resolution_clock::now().time_since_epoch());

  return (time_since_epoch / slot_duration) % nof_slots_per_hyper_system_frame;
}

ru_dummy_impl::ru_dummy_impl(const ru_dummy_configuration& config, ru_dummy_dependencies dependencies) noexcept :
  are_metrics_enabled(config.are_metrics_enabled),
  logger(dependencies.logger),
  executor(*dependencies.executor),
  timing_notifier(dependencies.timing_notifier),
  slot_duration(static_cast<unsigned>(config.time_scaling * 1000.0 / pow2(to_numerology_value(config.scs)))),
  max_processing_delay_slots(config.max_processing_delay_slots),
  current_slot(config.scs, config.max_processing_delay_slots),
  metrics_collector({})
{
  srsran_assert(config.max_processing_delay_slots > 0, "The maximum processing delay must be greater than 0.");

  sectors.reserve(config.nof_sectors);
  for (unsigned i_sector = 0; i_sector != config.nof_sectors; ++i_sector) {
    sectors.emplace_back(i_sector,
                         config.rx_rg_nof_prb,
                         config.rx_rg_nof_ports,
                         config.rx_prach_nof_ports,
                         config.dl_processing_delay,
                         logger,
                         dependencies.symbol_notifier,
                         dependencies.error_notifier);
  }

  metrics_collector = ru_dummy_metrics_collector(sectors);
}

void ru_dummy_impl::start()
{
  // Get initial system slot.
  uint64_t initial_system_slot =
      get_current_system_slot(slot_duration, current_slot.nof_slots_per_hyper_system_frame());
  current_slot = slot_point(current_slot.numerology(), initial_system_slot);

  // Start the loop execution.
  report_fatal_error_if_not(executor.execute([this]() { loop(); }), "Failed to execute loop method.");
}

void ru_dummy_impl::stop()
{
  // Stop each of the sectors.
  for (auto& sector : sectors) {
    sector.stop();
  }

  // Signal stop to asynchronous thread.
  stop_request = true;
  auto ft      = stop_promise.get_future();
  ft.wait();
}

void ru_dummy_impl::loop()
{
  // If stop has been requested, then return without deferring the loop task.
  if (stop_request) {
    stop_promise.set_value();
    return;
  }

  // Get the current system slot from the system time.
  uint64_t slot_count = get_current_system_slot(slot_duration, current_slot.nof_slots_per_hyper_system_frame());

  // Make sure a minimum time between loop executions without crossing boundaries.
  if (slot_count == current_slot.system_slot()) {
    std::this_thread::sleep_for(minimum_loop_time);
  }

  // Advance the current slot until it is equal to the slot given by the system time.
  while (slot_count != current_slot.system_slot()) {
    // Increment current slot.
    ++current_slot;

    // Notify new slot boundary.
    tti_boundary_context context;
    context.slot       = current_slot + max_processing_delay_slots;
    context.time_point = std::chrono::system_clock::now() + (slot_duration * max_processing_delay_slots);
    timing_notifier.on_tti_boundary(context);

    // Notify UL half slot.
    timing_notifier.on_ul_half_slot_boundary(current_slot);

    // Notify UL full slot.
    timing_notifier.on_ul_full_slot_boundary(current_slot);

    // Notify the slot boundary in all the sectors.
    for (auto& sector : sectors) {
      sector.new_slot_boundary(current_slot);
    }
  }

  // Feed back the execution of this task.
  report_fatal_error_if_not(executor.defer([this]() { loop(); }), "Failed to execute loop method.");
}
