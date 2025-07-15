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

#include "prs_generator_pool.h"

namespace srsran {

/// Concurrent PRS generator pool factory - instantiates PRS generators that can be executed concurrently.
class prs_generator_pool_factory : public prs_generator_factory
{
public:
  prs_generator_pool_factory(std::shared_ptr<prs_generator_factory> factory_, unsigned nof_concurrent_threads_) :
    factory(std::move(factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    srsran_assert(factory, "Invalid PRS generator factory.");
    srsran_assert(nof_concurrent_threads > 1, "Number of concurrent threads must be greater than one.");
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator> create() override
  {
    if (!pool) {
      std::vector<std::unique_ptr<prs_generator>> generators(nof_concurrent_threads);
      std::generate(generators.begin(), generators.end(), [this]() { return factory->create(); });
      pool = std::make_shared<prs_generator_pool::generator_pool_type>(generators);
    }

    return std::make_unique<prs_generator_pool>(pool);
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator> create(srslog::basic_logger& logger) override
  {
    if (!pool) {
      std::vector<std::unique_ptr<prs_generator>> generators(nof_concurrent_threads);
      std::generate(generators.begin(), generators.end(), [this, &logger]() { return factory->create(logger); });
      pool = std::make_shared<prs_generator_pool::generator_pool_type>(generators);
    }

    return std::make_unique<prs_generator_pool>(pool);
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator_validator> create_validator() override { return factory->create_validator(); }

private:
  /// Base factory.
  std::shared_ptr<prs_generator_factory> factory;
  /// Number of threads that use the instance.
  unsigned nof_concurrent_threads;
  /// Collection of PRS generators.
  std::shared_ptr<prs_generator_pool::generator_pool_type> pool;
};

} // namespace srsran
