/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../support/prach_context_repository.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul User-Plane PRACH data flow notifier.
class uplane_prach_data_flow_notifier
{
public:
  uplane_prach_data_flow_notifier(srslog::basic_logger&                      logger_,
                                  std::shared_ptr<prach_context_repository>  prach_context_repo_ptr_,
                                  std::shared_ptr<uplane_rx_symbol_notifier> notifier_) :
    logger(logger_),
    prach_context_repo_ptr(prach_context_repo_ptr_),
    prach_context_repo(*prach_context_repo_ptr),
    notifier_ptr(notifier_),
    notifier(*notifier_ptr)
  {
    srsran_assert(prach_context_repo_ptr, "Invalid PRACH context repository");
  }

  /// Notifies the prach for the given slot.
  void notify_prach(slot_point slot);

private:
  srslog::basic_logger&                      logger;
  std::shared_ptr<prach_context_repository>  prach_context_repo_ptr;
  prach_context_repository&                  prach_context_repo;
  std::shared_ptr<uplane_rx_symbol_notifier> notifier_ptr;
  uplane_rx_symbol_notifier&                 notifier;
};

} // namespace ofh
} // namespace srsran
