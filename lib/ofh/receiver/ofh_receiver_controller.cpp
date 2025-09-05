/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_receiver_controller.h"

using namespace srsran;
using namespace ofh;

void receiver_controller::start()
{
  msg_receiver.get_operation_controller().start();
  window_handler.start();
}

void receiver_controller::stop()
{
  window_handler.stop();
  msg_receiver.get_operation_controller().stop();
}
