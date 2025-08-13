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

#include "srsran/cu_cp/security_manager_config.h"
#include <map>

namespace srsran::srs_cu_cp {

/// UE security manager implementation
class ue_security_manager
{
public:
  ue_security_manager(const security_manager_config& cfg_);
  ~ue_security_manager() = default;

  // up_ue_security_manager
  [[nodiscard]] bool                        is_security_context_initialized() const;
  [[nodiscard]] security::sec_as_config     get_up_as_config() const;
  [[nodiscard]] security::sec_128_as_config get_up_128_as_config() const;
  [[nodiscard]] uint8_t                     get_ncc() const;

  // ngap_ue_security_manager
  bool               init_security_context(const security::security_context& sec_ctxt);
  [[nodiscard]] bool is_security_enabled() const;

  // rrc_ue_security_manager
  void                                       enable_security();
  [[nodiscard]] security::security_context   get_security_context() const;
  [[nodiscard]] security::sec_selected_algos get_security_algos() const;
  [[nodiscard]] security::sec_as_config      get_rrc_as_config() const;
  [[nodiscard]] security::sec_128_as_config  get_rrc_128_as_config() const;
  void                                       update_security_context(const security::security_context& sec_ctxt);
  void perform_horizontal_key_derivation(pci_t target_pci, unsigned target_ssb_arfcn);

private:
  security_manager_config    cfg;
  security::security_context sec_context;
  bool                       security_enabled = false;

  srslog::basic_logger& logger;
};

} // namespace srsran::srs_cu_cp
