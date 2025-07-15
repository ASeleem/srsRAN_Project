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

#include "downlink_processor_baseband_impl.h"
#include "srsran/adt/blocking_queue.h"
#include "srsran/phy/lower/processors/downlink/downlink_processor.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor.h"

namespace srsran {

/// Implements a software generic lower PHY downlink processor.
class downlink_processor_impl : public lower_phy_downlink_processor
{
public:
  /// \brief Constructs a software generic lower PHY downlink processor that can process downlink resource grids.
  /// \param[in] pdxch_proc        PDxCH processor.
  /// \param[in] amplitude_control Amplitude controller.
  /// \param[in] config            Downlink processor configuration.
  downlink_processor_impl(std::unique_ptr<pdxch_processor>                 pdxch_proc,
                          std::unique_ptr<amplitude_controller>            amplitude_control,
                          const downlink_processor_baseband_configuration& config);

  // See interface for documentation.
  void connect(downlink_processor_notifier& notifier, pdxch_processor_notifier& pdxch_notifier) override;

  // See interface for documentation.
  void stop() override { pdxch_proc->stop(); }

  // See interface for documentation.
  baseband_cfo_processor& get_cfo_control() override;

  // See interface for documentation.
  lower_phy_center_freq_controller& get_carrier_center_frequency_control() override;

  // See interface for documentation.
  lower_phy_tx_time_offset_controller& get_tx_time_offset_control() override;

  // See interface for documentation.
  pdxch_processor_request_handler& get_downlink_request_handler() override;

  // See interface for documentation.
  downlink_processor_baseband& get_baseband() override;

private:
  /// PDxCH processor.
  std::unique_ptr<pdxch_processor> pdxch_proc;
  /// Amplitude control.
  std::unique_ptr<amplitude_controller> amplitude_control;
  /// Baseband processor.
  downlink_processor_baseband_impl downlink_proc_baseband;
};

} // namespace srsran
