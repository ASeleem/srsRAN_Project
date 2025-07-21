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
#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/ran/resource_allocation/rb_bitmap.h"

namespace srsran {

class resource_grid_writer;

/// \brief Describes a PDCCH modulator interface.
///
/// \remark Defined in TS38.211 section 7.3.2 Physical downlink control channel (PDCCH).
/// \note The configuration assumes the CCE-to-REG and REG-to-PRB mapping is included in \c rb_mask.
class pdcch_modulator
{
public:
  /// Describes the necessary parameters to modulate a PDCCH transmission.
  struct config_t {
    /// Indicates the RBs used for the PDCCH transmission.
    crb_bitmap rb_mask;
    /// CORESET start symbol index.
    unsigned start_symbol_index;
    /// CORESET duration in symbols.
    unsigned duration;
    /// Higher layer parameter PDCCH-DMRS-ScramblingID if it is given, otherwise the physical cell identifier.
    unsigned n_id;
    /// Parameter \f$n_{RNTI}\f$ used for PDCCH data scrambling according to TS38.211 section 7.3.2.3.
    unsigned n_rnti;
    /// Scaling factor to apply to the resource elements according to PDCCH power allocation in TS38.213.
    float scaling;
    /// Precoding information for the PDCCH transmission.
    precoding_configuration precoding;
  };

  /// Default destructor.
  virtual ~pdcch_modulator() = default;

  /// \brief Modulates a PDCCH codeword according to TS38.211 Section 7.3.2.
  /// \param[out] grid   Resource grid writer interface.
  /// \param[in] data    PDCCH codeword to modulate.
  /// \param[in] config  Necessary parameters for modulating PDCCH.
  /// \remark The codeword length shall be consistent with the resource mapping.
  /// \remark An assertion is triggered if the number of precoding layers is not one.
  virtual void modulate(resource_grid_writer& grid, span<const uint8_t> data, const config_t& config) = 0;
};

} // namespace srsran
