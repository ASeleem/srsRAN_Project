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

#include <chrono>

namespace srsran {

/// Lower physical layer carrier frequency offset controller interface.
class lower_phy_cfo_controller
{
public:
  /// Data type used for point in time.
  using time_point = std::chrono::system_clock::time_point;

  /// Default destructor.
  virtual ~lower_phy_cfo_controller() = default;

  /// \brief Schedules a CFO command.
  /// \param time Time at which the CFO value is applied.
  /// \param cfo_Hz New CFO value in Hertz.
  /// \param cfo_drift_Hz_s New CFO drift value in Hertz per second.
  virtual bool schedule_cfo_command(time_point time, float cfo_Hz, float cfo_drift_Hz_s = 0) = 0;
};

} // namespace srsran
