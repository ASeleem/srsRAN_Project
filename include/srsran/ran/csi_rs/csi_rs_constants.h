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

namespace srsran {

/// Maximum number of \f$k_i\f$ frequency allocation reference values, according to TS 38.211 Table 7.4.1.5.3-1.
constexpr unsigned CSI_RS_MAX_NOF_K_INDEXES = 6;

/// Maximum number of ports that can be simultaneously used for CSI-RS, detailed in the TS38.211 Table 7.4.1.5.3-1
/// Row 18.
constexpr unsigned CSI_RS_MAX_NOF_PORTS = 32;

/// Maximum number of resource sets per resource configuration. See TS 38.331, \c
/// maxNrofNZP-CSI-RS-ResourceSetsPerConfig.
constexpr unsigned MAX_NOF_NZP_CSI_RS_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG = 16;

/// Maximum number of CSI SSB resource sets per resource configuration. See TS 38.331, \c
/// maxNrofCSI-SSB-ResourceSetsPerConfig.
constexpr unsigned MAX_NOF_CSI_SSB_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG = 1;

/// Maximum number of CSI IM resource sets per resource configuration. See TS 38.331, \c
/// maxNrofCSI-IM-ResourceSetsPerConfig.
constexpr unsigned MAX_NOF_CSI_IM_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG = 16;

/// Maximum number of resources per resource configuration. See TS 38.331, \c
/// maxNrofNZP-CSI-RS-ResourcesPerConfig.
constexpr unsigned MAX_NOF_NZP_CSI_RS_RESOURCES_PER_CONFIG = 128;

/// Maximum number of triggers for aperiodic CSI reporting. See TS 38.331, \c
/// maxNrOfCSI-AperiodicTriggers.
constexpr unsigned MAX_NOF_CSI_APERIODIC_TRIGGERS = 128;

/// Maximum number of report configurations per trigger state for aperiodic reporting. See TS 38.331, \c
/// maxNrofReportConfigPerAperiodicTrigger.
constexpr unsigned MAX_NOF_REPORT_CONFIG_PER_APERIODIC_TRIGGER = 16;

/// See TS 38.331, \c maxNrofAP-CSI-RS-ResourcesPerSet.
constexpr unsigned MAX_NOF_AP_CSI_RS_RESOURCES_PER_SET = 16;

/// Maximum number of triggers for semi persistent reporting on PUSCH. See TS 38.331, \c
/// maxNrOfSemiPersistentPUSCH-Triggers.
constexpr unsigned MAX_NOF_SEMI_PERSISTENT_PUSCH_TRIGGERS = 64;

} // namespace srsran
