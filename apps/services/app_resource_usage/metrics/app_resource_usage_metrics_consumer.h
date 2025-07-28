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

#include "apps/services/app_resource_usage/metrics/app_resource_usage_metrics.h"
#include "srsran/srslog/log_channel.h"

namespace srsran {

/// Log consumer for the resource usage metrics.
class resource_usage_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit resource_usage_metrics_consumer_log(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

/// JSON consumer for the resource usage metrics.
class resource_usage_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit resource_usage_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

} // namespace srsran
