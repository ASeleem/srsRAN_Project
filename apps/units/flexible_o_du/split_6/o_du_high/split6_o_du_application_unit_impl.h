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

#include "apps/units/flexible_o_du/flexible_o_du_application_unit.h"
#include "split6_o_du_unit_config.h"
#include "split6_plugin.h"

namespace srsran {

/// O-RAN DU Split6 application unit implementation.
class split6_o_du_application_unit_impl : public flexible_o_du_application_unit
{
public:
  split6_o_du_application_unit_impl(std::string_view app_name, std::unique_ptr<split6_plugin> plugin_);

  // See interface for documentation.
  void on_parsing_configuration_registration(CLI::App& app) override;

  // See interface for documentation.
  void on_configuration_parameters_autoderivation(CLI::App& app) override;

  // See interface for documentation.
  bool on_configuration_validation() const override;

  // See interface for documentation.
  void on_loggers_registration() override;

  // See interface for documentation.
  bool are_metrics_enabled() const override;

  // See interface for documentation.
  o_du_unit create_flexible_o_du_unit(const o_du_unit_dependencies& dependencies) override;

  // See interface for documentation.
  o_du_high_unit_config&       get_o_du_high_unit_config() override { return unit_cfg.odu_high_cfg; }
  const o_du_high_unit_config& get_o_du_high_unit_config() const override { return unit_cfg.odu_high_cfg; }

  // See interface for documentation.
  void dump_config(YAML::Node& node) const override;

  // See interface for documentation.
  void fill_worker_manager_config(worker_manager_config& config) override;

private:
  split6_o_du_unit_config        unit_cfg;
  std::unique_ptr<split6_plugin> plugin;
};

} // namespace srsran
