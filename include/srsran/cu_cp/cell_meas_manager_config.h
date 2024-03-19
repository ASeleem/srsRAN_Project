/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/band_helper.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/rrc/meas_types.h"
#include <map>
#include <vector>

namespace srsran {

namespace srs_cu_cp {

/// \brief Essential parameters required to configure serving cell measurements in the UE.
/// Note that some optional values need to be provided by the DU upon F1Setup.

struct serving_cell_meas_config {
  nr_cell_id_t nci; ///< The NR cell identifier.
  /// If not set in config must be provided by config update after DU attach.
  optional<gnb_id_t>           gnb_id;    ///< gNodeB identifier
  optional<pci_t>              pci;       ///< Physical cell identifier.
  optional<nr_band>            band;      ///< NR band.
  optional<rrc_ssb_mtc>        ssb_mtc;   ///< SSB measurement and timing config.
  optional<unsigned>           ssb_arfcn; ///< SSB ARFCN.
  optional<subcarrier_spacing> ssb_scs;   ///< SSB subcarrier spacing.
};

struct neighbor_cell_meas_config {
  nr_cell_id_t                 nci;            ///< The NR cell identifier.
  std::vector<report_cfg_id_t> report_cfg_ids; ///< The configured report configs
};

/// \brief Essential parameters required to configure serving and neighbor cell measurements in the UE.
/// Note that some optional values need to be provided by the DU upon F1Setup.
struct cell_meas_config {
  serving_cell_meas_config               serving_cell_cfg;       ///< Serving cell measurement config
  optional<report_cfg_id_t>              periodic_report_cfg_id; ///< The periodic report config
  std::vector<neighbor_cell_meas_config> ncells;                 ///< List of neighbor cells.
};

/// \brief Verifies required parameters are set. Returns true if config is valid, false otherwise.
bool is_complete(const serving_cell_meas_config& cfg);

/// \brief Cell manager configuration.
struct cell_meas_manager_cfg {
  std::map<nr_cell_id_t, cell_meas_config>     cells; // Measurement related configs for all known cells.
  std::map<report_cfg_id_t, rrc_report_cfg_nr> report_config_ids;
};

/// \brief Validates configuration but doesn't verify if all provided cells have complete configuration (yet). Returns
/// true if config is valid, false otherwise.
bool is_valid_configuration(const cell_meas_manager_cfg& cfg);

/// \brief Same as config validation but additionally verfies that the measurement related parameters are present for
/// all cells.
bool is_complete(const cell_meas_manager_cfg& cfg);

} // namespace srs_cu_cp

} // namespace srsran

namespace fmt {

// Cell meas config formatter
template <>
struct formatter<srsran::srs_cu_cp::cell_meas_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_cu_cp::cell_meas_config cfg, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    std::string ncell_str = "[ ";
    for (const auto& ncell : cfg.ncells) {
      ncell_str += fmt::format("{:#x} ", ncell.nci);
    }
    ncell_str = ncell_str + "]";

    return format_to(
        ctx.out(),
        "nci={:#x} complete={} gnb_id={} pci={} band={} ssb_arfcn={} ssb_scs={} ncells={}",
        cfg.serving_cell_cfg.nci,
        is_complete(cfg.serving_cell_cfg) ? "yes" : "no",
        cfg.serving_cell_cfg.gnb_id.has_value() ? to_string(cfg.serving_cell_cfg.gnb_id.value().id) : "?",
        cfg.serving_cell_cfg.pci.has_value() ? to_string(cfg.serving_cell_cfg.pci.value()) : "?",
        cfg.serving_cell_cfg.band.has_value() ? to_string(nr_band_to_uint(cfg.serving_cell_cfg.band.value())) : "?",
        cfg.serving_cell_cfg.ssb_arfcn.has_value() ? to_string(cfg.serving_cell_cfg.ssb_arfcn.value()) : "?",
        cfg.serving_cell_cfg.ssb_scs.has_value() ? to_string(cfg.serving_cell_cfg.ssb_scs.value()) : "?",
        ncell_str);
  }
};

} // namespace fmt
