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

#include "ofh_receiver_data_flow_metrics.h"
#include "srsran/ofh/ethernet/ethernet_receiver_metrics.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul received messages metrics.
struct received_messages_metrics {
  /// Number of OFH messages received on time.
  unsigned nof_on_time_messages;
  /// Number of OFH messages received early.
  unsigned nof_early_messages;
  /// Number of OFH messages received late.
  unsigned nof_late_messages;
  /// Earliest received message in symbols unit.
  int ealiest_rx_msg_in_symbols;
  /// Latest received message in symbols unit.
  unsigned latest_rx_msg_in_symbols;
};

/// Open Fronthaul reception window closing metrics.
struct closed_rx_window_metrics {
  /// Number of requested uplink symbols that were not fully received within the expected reception window.
  unsigned nof_missing_uplink_symbols;
  /// Number of PRACH context that were not fully received within the expected reception window.
  unsigned nof_missing_prach_contexts;
};

/// Open Fronthaul received messages decoding performance metrics.
struct message_decoding_performance_metrics {
  rx_data_flow_perf_metrics data_processing_metrics;
  rx_data_flow_perf_metrics prach_processing_metrics;
  unsigned                  nof_dropped_messages;
  unsigned                  nof_skipped_messages;
};

/// Open Fronthaul receiver metrics.
struct receiver_metrics {
  closed_rx_window_metrics             closed_window_metrics;
  received_messages_metrics            rx_messages_metrics;
  message_decoding_performance_metrics rx_decoding_perf_metrics;
  ether::receiver_metrics              eth_receiver_metrics;
};

} // namespace ofh
} // namespace srsran
