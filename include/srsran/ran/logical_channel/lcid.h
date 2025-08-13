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

#include <cstdint>
#include <type_traits>

namespace srsran {

/// Logical Channel Identity used to associate one logical channel to the corresponding RLC bearer. Values (0..32)
enum lcid_t : uint16_t {
  LCID_SRB0        = 0,
  LCID_SRB1        = 1,
  LCID_SRB2        = 2,
  LCID_SRB3        = 3,
  LCID_MIN_DRB     = 4,
  LCID_MAX_DRB     = 32,
  MAX_NOF_RB_LCIDS = 33,
  INVALID_LCID     = 64
};

constexpr lcid_t uint_to_lcid(std::underlying_type_t<lcid_t> val)
{
  return static_cast<lcid_t>(val);
}

/// Maximum value of Logical Channel ID.
/// \remark See TS 38.331, maxLC-ID.
constexpr lcid_t MAX_LCID = LCID_MAX_DRB;

constexpr bool is_srb(lcid_t lcid)
{
  return lcid <= LCID_SRB3;
}

inline bool is_lcid_valid(lcid_t lcid)
{
  return lcid <= MAX_LCID;
}

} // namespace srsran
