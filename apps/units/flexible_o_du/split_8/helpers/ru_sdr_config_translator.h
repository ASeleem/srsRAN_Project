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

#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_configs.h"
#include "srsran/ru/generic/ru_generic_configuration.h"

namespace srsran {

struct ru_sdr_unit_config;
struct worker_manager_config;

/// Converts and returns the given RU SDR application unit configuration to a SDR RU configuration.
ru_generic_configuration generate_ru_sdr_config(const ru_sdr_unit_config&                        ru_cfg,
                                                span<const flexible_o_du_ru_config::cell_config> cells,
                                                unsigned max_processing_delay_slots);

/// Fills the SDR worker manager parameters of the given worker manager configuration.
void fill_sdr_worker_manager_config(worker_manager_config& config, const ru_sdr_unit_config& ru_cfg);

} // namespace srsran
