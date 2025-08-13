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

#include "srsran/gateways/sctp_network_gateway.h"
#include <memory>
#include <utility>

namespace srsran {

class task_executor;

struct sctp_network_gateway_creation_message {
  sctp_network_gateway_creation_message(sctp_network_connector_config          config_,
                                        sctp_network_gateway_control_notifier& ctrl_notifier_,
                                        network_gateway_data_notifier&         data_notifier_,
                                        task_executor&                         io_rx_executor_) :
    config(std::move(config_)),
    ctrl_notifier(ctrl_notifier_),
    data_notifier(data_notifier_),
    io_rx_executor(io_rx_executor_)
  {
  }
  sctp_network_connector_config          config;
  sctp_network_gateway_control_notifier& ctrl_notifier;
  network_gateway_data_notifier&         data_notifier;
  task_executor&                         io_rx_executor;
};

/// Creates an instance of an network gateway
std::unique_ptr<sctp_network_gateway> create_sctp_network_gateway(const sctp_network_gateway_creation_message& msg);

} // namespace srsran
