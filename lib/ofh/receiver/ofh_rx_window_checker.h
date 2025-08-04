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

#include "srsran/ofh/receiver/ofh_receiver_metrics.h"
#include "srsran/ofh/receiver/ofh_receiver_metrics_collector.h"
#include "srsran/ofh/receiver/ofh_receiver_timing_parameters.h"
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/srslog/logger.h"
#include <atomic>

namespace srsran {
namespace ofh {

/// \brief Open Fronthaul reception window checker.
///
/// Checks if the given slot and symbol is within the reception window or not. The window checker also collects
/// statistics and prints them every second.
class rx_window_checker : public ota_symbol_boundary_notifier
{
  /// Helper class that represents the reception window statistics.
  class rx_window_checker_statistics
  {
    static constexpr unsigned NOF_BITS_PER_COUNTER = 21U;

    /// \brief Latest packet received.
    ///
    /// This value represents the number of symbols after the reception window closed when a packet was received. A
    /// value of 0 means all the packets were received within the reception window.
    std::atomic<int32_t> latest_packet_in_symbols{0};
    /// \brief Earliest packet received.
    ///
    /// This value represents the number of symbols before the reception window opened when a packet was received. A
    /// value of 0 means all the packets were received within the reception window.
    std::atomic<int32_t>  earliest_packet_in_symbols{0};
    std::atomic<uint64_t> on_time_counter{0};
    std::atomic<uint64_t> early_counter{0};
    std::atomic<uint64_t> late_counter{0};
    uint64_t              last_on_time_value_printed = 0U;
    uint64_t              last_early_value_printed   = 0U;
    uint64_t              last_late_value_printed    = 0U;

  public:
    /// Collect the statistics.
    void collect_metrics(received_messages_metrics& metrics);

    /// Functions to increment the counters.
    void increment_on_time_counter() { on_time_counter.fetch_add(1, std::memory_order_relaxed); }
    void increment_early_counter() { early_counter.fetch_add(1, std::memory_order_relaxed); }
    void increment_late_counter() { late_counter.fetch_add(1, std::memory_order_relaxed); }
    void update_latest_packet(int32_t value)
    {
      int32_t latest = latest_packet_in_symbols.load(std::memory_order_relaxed);
      if (value < latest) {
        return;
      }

      // Update value.
      if (!latest_packet_in_symbols.compare_exchange_strong(latest, value, std::memory_order_relaxed)) {
        latest_packet_in_symbols.compare_exchange_strong(latest, value, std::memory_order_relaxed);
      }
    }

    void update_earliest_packet(int value)
    {
      int32_t earliest = earliest_packet_in_symbols.load(std::memory_order_relaxed);
      if (value < earliest) {
        return;
      }

      // Update value.
      if (!earliest_packet_in_symbols.compare_exchange_strong(earliest, value, std::memory_order_relaxed)) {
        earliest_packet_in_symbols.compare_exchange_strong(earliest, value, std::memory_order_relaxed);
      }
    }

    /// Getters to the message counters.
    uint64_t nof_on_time_messages() const { return on_time_counter.load(std::memory_order_relaxed); }
    uint64_t nof_early_messages() const { return early_counter.load(std::memory_order_relaxed); }
    uint64_t nof_late_messages() const { return late_counter.load(std::memory_order_relaxed); }
  };

  const rx_window_timing_parameters timing_parameters;
  const bool                        is_disabled;
  rx_window_checker_statistics      statistics;
  std::atomic<uint32_t>             slot_raw_value;

public:
  rx_window_checker(bool is_enabled, const rx_window_timing_parameters& params);

  // See interface for documentation.
  void on_new_symbol(const slot_symbol_point_context& symbol_point_context) override;

  // See interface for documentation.
  void collect_metrics(received_messages_metrics& metrics);

  /// Returns true if the Rx window checker is disabled, otherwise returns false.
  bool disabled() const { return is_disabled; }

  /// Updates the Rx window statistics.
  void update_rx_window_statistics(slot_symbol_point symbol_point);

  /// Getters to the number of messages.
  uint64_t nof_on_time_messages() const { return statistics.nof_on_time_messages(); }
  uint64_t nof_early_messages() const { return statistics.nof_early_messages(); }
  uint64_t nof_late_messages() const { return statistics.nof_late_messages(); }
};

} // namespace ofh
} // namespace srsran
