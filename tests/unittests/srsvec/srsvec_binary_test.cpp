/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srsvec/binary.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);

using namespace srsran;

template <typename T, unsigned RANGE>
void test_binary_xor(std::size_t N)
{
  std::uniform_int_distribution<T> dist(0, RANGE);

  std::vector<T> x(N);
  for (T& v : x) {
    v = dist(rgen);
  }

  std::vector<T> y(N);
  for (T& v : y) {
    v = dist(rgen);
  }

  std::vector<T> z(N);

  srsvec::binary_xor(z, x, y);

  for (size_t i = 0; i != N; ++i) {
    T gold_z = x[i] ^ y[i];
    TESTASSERT_EQ(gold_z, z[i]);
  }
}

template <typename T, unsigned RANGE>
void test_binary_and(std::size_t N)
{
  std::uniform_int_distribution<T> dist(0, RANGE);

  std::vector<T> x(N);
  for (T& v : x) {
    v = dist(rgen);
  }

  std::vector<T> y(N);
  for (T& v : y) {
    v = dist(rgen);
  }

  std::vector<T> z(N);

  srsvec::binary_and(z, x, y);

  for (size_t i = 0; i != N; ++i) {
    T gold_z = x[i] & y[i];
    TESTASSERT_EQ(gold_z, z[i]);
  }
}

template <typename T, unsigned RANGE>
void test_binary_or(std::size_t N)
{
  std::uniform_int_distribution<T> dist(0, RANGE);

  std::vector<T> x(N);
  for (T& v : x) {
    v = dist(rgen);
  }

  std::vector<T> y(N);
  for (T& v : y) {
    v = dist(rgen);
  }

  std::vector<T> z(N);

  srsvec::binary_or(z, x, y);

  for (size_t i = 0; i != N; ++i) {
    T gold_z = x[i] | y[i];
    TESTASSERT_EQ(gold_z, z[i]);
  }
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 257};

  for (std::size_t N : sizes) {
    test_binary_xor<uint8_t, UINT8_MAX>(N);
    test_binary_and<uint8_t, UINT8_MAX>(N);
    test_binary_or<uint8_t, UINT8_MAX>(N);
  }
}
