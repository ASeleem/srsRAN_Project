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

namespace srsran {
namespace fapi {

class operation_controller;
class slot_data_message_notifier;
class error_message_notifier;
class slot_last_message_notifier;
class slot_message_gateway;
class slot_time_message_notifier;

/// \brief FAPI message interface collection interface.
///
/// This interface gives access to the FAPI message interfaces.
class fapi_message_interface_collection
{
public:
  virtual ~fapi_message_interface_collection() = default;

  /// Returns the slot message gateway of this adaptor.
  virtual slot_message_gateway& get_slot_message_gateway() = 0;

  /// Returns the last message notifier of this adaptor.
  virtual slot_last_message_notifier& get_slot_last_message_notifier() = 0;

  /// Sets the slot time message notifier to the given notifier.
  virtual void set_slot_time_message_notifier(slot_time_message_notifier& notifier_) = 0;

  /// Sets the slot data message notifier to the given notifier.
  virtual void set_slot_data_message_notifier(slot_data_message_notifier& notifier_) = 0;

  /// Sets the error message notifier to the given notifier.
  virtual void set_error_message_notifier(error_message_notifier& notifier_) = 0;
};

/// \brief FAPI adaptor interface.
///
/// The FAPI adaptor provides the functionality to connect to a FAPI interface and interact with it using the
/// controllers provided.
class fapi_adaptor
{
public:
  virtual ~fapi_adaptor() = default;

  /// Returns the operation controller of this FAPI adaptor.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns the message interface collection of this FAPI adaptor.
  virtual fapi_message_interface_collection& get_message_interface_collection() = 0;
};

} // namespace fapi
} // namespace srsran
