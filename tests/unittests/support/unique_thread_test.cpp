/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/unique_thread.h"
#include "srsran/support/test_utils.h"
#include "fmt/std.h"
#include <mutex>
#include <unistd.h>

using namespace srsran;

void test_uninit_unique_thread()
{
  test_delimit_logger delimit{"Uninitialized unique thread"};

  unique_thread t;
  TESTASSERT(not t.running());
  TESTASSERT_EQ(std::string(""), t.get_name());
  TESTASSERT_NEQ(std::string(this_thread_name()), t.get_name());

  t.print_priority();

  // Join is a NO-OP.
  t.join();
  TESTASSERT(not t.running());
}

void test_init_unique_thread()
{
  test_delimit_logger delimit{"Initialized unique thread"};

  unique_thread t;
  TESTASSERT(not t.running());

  // Action: start thread.
  std::atomic<bool> running{true};
  std::string       t_name;
  t = unique_thread("T1", [&running]() {
    TESTASSERT_EQ(std::string("T1"), this_thread_name());

    print_this_thread_priority();

    fmt::print("Thread [{}:{}] completed.\n", this_thread_name(), std::this_thread::get_id());
    running = false;
  });
  TESTASSERT_EQ(std::string(t.get_name()), "T1");
  TESTASSERT_NEQ(std::string(t.get_name()), this_thread_name());

  while (running) {
    ::usleep(100);
  }
  fmt::print("Main Thread [{}:{}] completed.\n", this_thread_name(), std::this_thread::get_id());
}

// Note: The thread priority is only set if this thread runs in sudo mode.
void test_init_unique_thread_prio()
{
  test_delimit_logger delimit{"Initialized unique thread with priority"};

  unique_thread t("T1", srsran::os_thread_realtime_priority::max(), []() { print_this_thread_priority(); });
}

int main()
{
  test_uninit_unique_thread();
  test_init_unique_thread();
  test_init_unique_thread_prio();
}
