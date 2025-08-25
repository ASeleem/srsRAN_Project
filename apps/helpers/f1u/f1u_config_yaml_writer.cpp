/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "apps/helpers/f1u/f1u_config_yaml_writer.h"
#include "apps/helpers/f1u/f1u_appconfig.h"
#include "apps/helpers/network/udp_config_yaml_writer.h"
#include <yaml-cpp/yaml.h>

using namespace srsran;

static void fill_cu_up_f1u_socket_entry(YAML::Node& node, const f1u_socket_appconfig& config)
{
  node["bind_addr"] = config.bind_addr;
  node["ext_addr"]  = config.udp_config.ext_addr;
  fill_udp_config_in_yaml_schema(node["udp"], config.udp_config);
}

/// Fills the UDP configuration in the given YAML node.
static void fill_f1u_socket_section(YAML::Node& node, const std::vector<f1u_socket_appconfig>& sock_cfg)
{
  auto sock_node = node["socket"];
  for (const auto& cfg : sock_cfg) {
    YAML::Node node_sock;
    fill_cu_up_f1u_socket_entry(node_sock, cfg);
    sock_node.push_back(node_sock);
  }
}

void srsran::fill_f1u_config_yaml_schema(YAML::Node& node, const f1u_sockets_appconfig& config)
{
  YAML::Node f1u_node = node["f1u"];
  node["bind_port"]   = config.bind_port;
  node["peer_port"]   = config.peer_port;
  fill_f1u_socket_section(f1u_node, config.f1u_socket_cfg);
}
