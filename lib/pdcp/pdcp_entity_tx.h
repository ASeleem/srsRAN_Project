/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "pdcp_entity_tx_rx_base.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/byte_buffer_slice_chain.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include "srsgnb/pdcp/pdcp_tx.h"
#include "srsgnb/ran/bearer_logger.h"
#include "srsgnb/security/security.h"

namespace srsgnb {

/// PDCP TX state variables,
/// TS 38.323, section 7.1
struct pdcp_tx_state {
  /// This state variable indicates the COUNT value of the next PDCP SDU to be transmitted. The initial value is 0,
  /// except for SRBs configured with state variables continuation.
  uint32_t tx_next;
};

/// Base class used for transmitting PDCP bearers.
/// It provides interfaces for the PDCP bearers, for the higher and lower layers
class pdcp_entity_tx : public pdcp_entity_tx_rx_base,
                       public pdcp_tx_upper_data_interface,
                       public pdcp_tx_lower_interface
{
public:
  pdcp_entity_tx(uint32_t                        ue_index,
                 lcid_t                          lcid,
                 pdcp_config::pdcp_tx_config     cfg_,
                 pdcp_tx_lower_notifier&         lower_dn,
                 pdcp_tx_upper_control_notifier& upper_cn) :
    pdcp_entity_tx_rx_base(cfg_.sn_size),
    logger("PDCP", ue_index, lcid),
    cfg(cfg_),
    lower_dn(lower_dn),
    upper_cn(upper_cn)
  {
  }

  /*
   * SDU/PDU handlers
   */
  void handle_sdu(byte_buffer buf) final;

  /*
   * Header helpers
   */
  bool write_data_pdu_header(byte_buffer& buf, uint32_t count);

  /*
   * Testing helpers
   */
  void set_state(pdcp_tx_state st_) { st = st_; };

private:
  bearer_logger               logger;
  pdcp_config::pdcp_tx_config cfg;

  pdcp_tx_lower_notifier&         lower_dn;
  pdcp_tx_upper_control_notifier& upper_cn;

  void stop_discard_timer(uint32_t count) final {}

  pdcp_tx_state     st      = {};
  sec_128_as_config sec_cfg = {};

  pdcp_tx_direction integrity_direction = pdcp_tx_direction::none;
  pdcp_tx_direction ciphering_direction = pdcp_tx_direction::none;

  /// Apply ciphering and integrity protection to the payload
  void apply_ciphering_and_integrity_protection(byte_buffer& buf, uint32_t count);
  void integrity_generate(byte_buffer& buf, uint32_t count, sec_mac& mac);

  /*
   * RB helpers
   */
  bool is_srb() { return cfg.rb_type == pdcp_rb_type::srb; }
  bool is_drb() { return cfg.rb_type == pdcp_rb_type::drb; }
};
} // namespace srsgnb
