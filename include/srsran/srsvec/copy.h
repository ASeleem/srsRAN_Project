/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Sequence copy.

#pragma once

#include "srsran/srsvec/type_traits.h"
#include "srsran/srsvec/types.h"

namespace srsran {
namespace srsvec {

/// \brief Copies a sequence into a different object.
///
/// \tparam     T    Type of the destination sequence. (must be compatible with a span).
/// \tparam     U    Type of the original sequence (must be compatible with a span).
/// \param[out] dst  Copied sequence.
/// \param[in]  src  Original sequence.
template <typename T, typename U>
void copy(T&& dst, const U& src)
{
  static_assert(is_span_compatible<T>::value, "Template type is not compatible with a span");
  static_assert(is_span_compatible<U>::value, "Template type is not compatible with a span");
  srsran_srsvec_assert_size(dst, src);

  std::copy(src.begin(), src.end(), dst.begin());
}

} // namespace srsvec
} // namespace srsran
