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

#include "srsran/adt/span.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/ssb/ssb_properties.h"
#include <array>

namespace srsran {

/// PBCH encoder interface.
class pbch_encoder
{
public:
  /// Generated payload length. TS38.212 refers to it as \e A.
  static const unsigned A = 32;
  /// Higher layer generated payload length. TS38.212 refers to it as \e A_hat.
  static const unsigned A_HAT = 24;
  /// CRC length in bits.
  static const unsigned CRC_LEN = 24;
  /// Payload size plus CRC length.
  static const unsigned B = (A + CRC_LEN);
  /// Number of rate matched bits.
  static const unsigned E = 864;
  /// Polar encoder order, logarithmic representation.
  static const unsigned POLAR_N_MAX_LOG = 9;
  /// Number of encoded bits.
  static const unsigned POLAR_N_MAX = (1U << POLAR_N_MAX_LOG);

  /// Describes a PBCH message to encode.
  struct pbch_msg_t {
    /// Physical cell identifier.
    pci_t N_id;
    /// SSB candidate index in a 5ms burst.
    unsigned ssb_idx;
    /// Maximum number of SS/PBCH block candidates in a 5ms burst, described in TS38.213 Section 4.1.
    unsigned L_max;
    /// Flag: true if the SS/PBCH block transmission is in an odd half frame, false otherwise.
    bool hrf;
    /// Actual PBCH payload provided by higher layers.
    std::array<uint8_t, A_HAT> payload;
    /// System Frame Number.
    unsigned sfn;
    /// \brief Subcarrier offset.
    ///
    /// Corresponds to parameter \f$k_{SSB}\f$ as defined in TS38.211 Section 7.4.3.1. The value range is {0, ..., 11}
    /// if \f$L_{MAX} = 64\f$, {0, ..., 23} otherwise.
    ssb_subcarrier_offset k_ssb;
  };

  /// Default destructor.
  virtual ~pbch_encoder() = default;

  /// \brief Encodes a PBCH message.
  /// \param[out] encoded Encoded data.
  /// \param[in]  pbch_msg PBCH message to encode.
  virtual void encode(span<uint8_t> encoded, const pbch_msg_t& pbch_msg) = 0;
};

} // namespace srsran
