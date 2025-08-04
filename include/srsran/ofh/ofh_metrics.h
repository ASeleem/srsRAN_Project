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

#include "srsran/adt/static_vector.h"
#include "srsran/ofh/receiver/ofh_receiver_metrics.h"
#include "srsran/ofh/timing/ofh_timing_metrics.h"
#include "srsran/ofh/transmitter/ofh_transmitter_metrics.h"
#include "srsran/ran/gnb_constants.h"
#include <chrono>

namespace srsran {
namespace ofh {

/// Open Fronthaul sector metrics.
struct sector_metrics {
  /// Sector identifier.
  unsigned sector_id;
  /// Receiver metrics.
  receiver_metrics rx_metrics;
  /// Transmitter metrics.
  transmitter_metrics tx_metrics;
  /// Metrics period.
  std::chrono::milliseconds metrics_period_ms;
};

/// Open Fronthaul metrics.
struct metrics {
  /// Timestamp of the OFH RU metrics.
  std::chrono::time_point<std::chrono::system_clock> timestamp;
  /// Timing metrics.
  timing_metrics timing;
  /// Metrics for every sector.
  static_vector<sector_metrics, MAX_CELLS_PER_DU> sectors;
};

} // namespace ofh
} // namespace srsran
