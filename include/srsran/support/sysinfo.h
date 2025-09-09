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

#include "srsran/srslog/logger.h"
#include <optional>

namespace srsran {

struct os_sched_affinity_bitmask;

/// \brief Check whether the CPU scaling governor is set to performance.
///
/// \param[in] logger to print warnings.
/// \return True if we were able to read the sysfs scaling governor information.
bool check_cpu_governor(srslog::basic_logger& logger);

/// \brief Check whether the DRM KMS polling is set.
///
/// \param[in] logger to print warnings.
/// \return True if we were able to read the sysfs for the DRM KMS polling information.
bool check_drm_kms_polling(srslog::basic_logger& logger);

} // namespace srsran
