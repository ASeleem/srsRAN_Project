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

#include "srsran/srsvec/types.h"

namespace srsran {
namespace srsvec {

void prod(span<cf_t> z, span<const cf_t> x, span<const cf_t> y);
void prod(span<cf_t> z, span<const cf_t> x, span<const float> y);
void prod(span<cf_t> z, span<const float> x, span<const cf_t> y);
void prod(span<float> z, span<const float> x, span<const float> y);
void prod(span<int16_t> z, span<const int16_t> x, span<const int16_t> y);

void prod_conj(span<cf_t> z, span<const cf_t> x, span<const cf_t> y);

/// \brief Multiplies a given signal by a complex exponential.
///
/// \param[out] out           Resultant complex samples.
/// \param[in]  in            Input complex samples.
/// \param[in]  norm_freq     Normalized frequency.
/// \param[in]  initial_phase Initial phase in radians.
void prod_cexp(span<cf_t> out, span<const cf_t> in, float norm_freq, float initial_phase = 0.0F);

} // namespace srsvec
} // namespace srsran
