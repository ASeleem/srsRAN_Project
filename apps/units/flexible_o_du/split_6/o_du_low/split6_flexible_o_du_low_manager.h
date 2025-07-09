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

#include "cell_operation_request_impl.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/fapi/cell_configurator_plugin.h"
#include "srsran/fapi/fapi_config_message_interface_collection.h"
#include <memory>

namespace srsran {

/// Split 6 flexible O-DU low manager dependencies.
struct split6_flexible_o_du_low_manager_dependencies {
  std::unique_ptr<split6_flexible_o_du_low_factory>               factory_odu_low;
  std::unique_ptr<fapi::cell_configurator_plugin>                 cell_plugin;
  std::unique_ptr<fapi::fapi_config_message_interface_collection> config_interface_collection;
};

/// \brief Split 6 flexible O-DU low manager.
///
/// This is the class returned by the application unit when it creates the O-DU low.
/// This object:
///  - Manages/owns the configuration plugin and the FAPI configuration related objects.
///  - Controls the cell creation/destruction using the cell operation request handler implementation.
class split6_flexible_o_du_low_manager : public du_operation_controller
{
public:
  explicit split6_flexible_o_du_low_manager(split6_flexible_o_du_low_manager_dependencies&& dependencies);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::unique_ptr<split6_flexible_o_du_low_factory>               factory_odu_low;
  cell_operation_request_handler_impl                             cell_controller;
  std::unique_ptr<fapi::cell_configurator_plugin>                 cell_plugin;
  std::unique_ptr<fapi::fapi_config_message_interface_collection> config_interface_collection;
};

} // namespace srsran
