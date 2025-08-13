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

#include "srsran/ran/resource_block.h"

namespace srsran {
namespace pdcch_constants {

/// Defines the number if resource blocks per frequency resource in a CORESET as per TS38.331 in ControlResourceSet
/// information element.
constexpr unsigned NOF_RB_PER_FREQ_RESOURCE = 6;

/// Defines the maximum number of frequency resources in a CORESET as per TS38.331 in ControlResourceSet
/// information element.
constexpr unsigned MAX_NOF_FREQ_RESOURCES = MAX_NOF_PRBS / NOF_RB_PER_FREQ_RESOURCE;

/// Maximum number of DCIs in a single transmission (it is not specified by the TS).
constexpr unsigned MAX_NOF_DCI = 1;

/// Maximum payload size in bits (it is not specified by the TS).
constexpr unsigned MAX_DCI_PAYLOAD_SIZE = 128;

/// Number of REs used for data in per RB and symbol as per TS38.211 section 7.3.2.5.
constexpr unsigned NOF_RE_PDCCH_PER_RB = NOF_SUBCARRIERS_PER_RB - 3;

/// Number of REGs per CCE as per TS38.211 Section 7.3.2.2.
constexpr unsigned NOF_REG_PER_CCE = 6;

/// Number of CRC bits that are attached to a PDCCH, as per TS38.211 Section 7.1.3.
constexpr unsigned NOF_CRC_BITS = 24U;

/// Maximum CORESET duration in symbols as per TS38.211 Section 7.3.2.2. and TS 38.331 "maxCoReSetDuration".
constexpr unsigned MAX_CORESET_DURATION = 3;

/// Maximum aggregation level as per TS38.211 Table 7.3.2.1-1.
constexpr unsigned MAX_AGGREGATION_LEVEL = 16;

/// \brief Maximum number of RBs that can be used for a single PDCCH transmission.
///
/// It coincides with the maximum number of REG that can be used for a single transmission.
constexpr unsigned MAX_NOF_RB_PDCCH = MAX_AGGREGATION_LEVEL * NOF_REG_PER_CCE;

/// Maximum number of REs for data in a single transmission.
constexpr unsigned MAX_NOF_RE_PDCCH = MAX_NOF_RB_PDCCH * NOF_RE_PDCCH_PER_RB;

/// Bits per symbol for QPSK, the modulation used for PDCCH, as per TS38.211 Section 7.3.2.4.
constexpr unsigned QPSK_BITS_PER_SYMBOL = 2;

/// Define the maximum number of encoded data bits in a transmission.
constexpr unsigned MAX_NOF_BITS = MAX_NOF_RE_PDCCH * QPSK_BITS_PER_SYMBOL;

/// Define the number of encoded data bits per CCE in a transmission.
constexpr unsigned NOF_BITS_PER_CCE = NOF_REG_PER_CCE * NOF_RE_PDCCH_PER_RB * QPSK_BITS_PER_SYMBOL;

} // namespace pdcch_constants
} // namespace srsran
