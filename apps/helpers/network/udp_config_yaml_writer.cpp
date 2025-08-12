/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "udp_config_yaml_writer.h"
#include "udp_appconfig.h"

using namespace srsran;

/// Fills the UDP configuration in the given YAML node.
void srsran::fill_udp_config_in_yaml_schema(YAML::Node node, const udp_appconfig& config)
{
  node["max_rx_msgs"]    = config.rx_max_msgs;
  node["pool_threshold"] = config.pool_threshold;
}
