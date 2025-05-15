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

#include "ofh_transmitter_data_flow_metrics.h"
#include "srsran/ofh/ethernet/ethernet_transmitter_metrics.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul transmitter downlink processing metrics.
struct transmitter_dl_metrics {
  /// OFH transmitter downlink C-Plane processing metrics.
  tx_data_flow_perf_metrics dl_cp_metrics;
  /// OFH transmitter U-Plane processing metrics.
  tx_data_flow_perf_metrics dl_up_metrics;
  /// Number of late downlink resource grids received from the PHY.
  unsigned nof_late_dl_grids;
};

/// Open Fronthaul transmitter uplink requests processing metrics.
struct transmitter_ul_metrics {
  /// Open Fronthaul transmitter uplink C-Plane processing metrics.
  tx_data_flow_perf_metrics ul_cp_metrics;
  /// Number of late uplink request.
  unsigned nof_late_ul_requests;
};

/// Open Fronthaul message transmitter metrics.
struct message_transmitter_metrics {
  /// Message enqueueing and transmission statistics.
  float message_tx_min_latency_us;
  float message_tx_max_latency_us;
  float message_tx_avg_latency_us;

  /// CPU usage in microseconds of the message transmitter processing.
  float cpu_usage_us;
};

/// Open Fronthaul transmitter metrics.
struct transmitter_metrics {
  transmitter_dl_metrics      dl_metrics;
  transmitter_ul_metrics      ul_metrics;
  message_transmitter_metrics message_tx_metrics;
  ether::transmitter_metrics  eth_transmitter_metrics;
};

} // namespace ofh
} // namespace srsran
