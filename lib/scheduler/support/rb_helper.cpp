/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rb_helper.h"
#include "srsran/ran/resource_block.h"

using namespace srsran;

template <typename BitsetTag, typename IntervalTag>
static interval<unsigned, false, IntervalTag>
find_next_empty_interval_impl(const bounded_bitset<MAX_NOF_PRBS, false, BitsetTag>& used_rb_bitmap,
                              interval<unsigned, false, IntervalTag>                search_limits)
{
  // Restrict the search to the bitmap dimensions.
  const interval<unsigned, false, IntervalTag> bitset_limits = {0, used_rb_bitmap.size()};
  search_limits.intersect(bitset_limits);

  int rb_start = used_rb_bitmap.find_lowest(search_limits.start(), search_limits.stop(), false);
  if (rb_start >= 0) {
    int rb_end = used_rb_bitmap.find_lowest(rb_start + 1, search_limits.stop(), true);
    return {static_cast<unsigned>(rb_start), rb_end >= 0 ? static_cast<unsigned>(rb_end) : search_limits.stop()};
  }
  return {};
}

template <typename BitsetTag, typename IntervalTag>
static interval<unsigned, false, IntervalTag>
find_empty_interval_of_length_impl(const bounded_bitset<MAX_NOF_PRBS, false, BitsetTag>& used_rb_bitmap,
                                   unsigned                                              nof_rbs,
                                   interval<unsigned, false, IntervalTag>                search_limits)
{
  using interval_t = interval<unsigned, false, IntervalTag>;
  // Restrict the search to the bitmap dimensions.
  const interval_t bitset_limits = {0, used_rb_bitmap.size()};
  search_limits.intersect(bitset_limits);

  interval_t max_interv;
  do {
    interval_t interv = find_next_empty_interval_impl(used_rb_bitmap, search_limits);
    if (interv.empty()) {
      break;
    }
    if (interv.length() >= nof_rbs) {
      max_interv.set(interv.start(), interv.start() + nof_rbs);
      break;
    }
    if (interv.length() > max_interv.length()) {
      max_interv = interv;
    }
    search_limits.displace_to(interv.stop() + 1);
  } while (not search_limits.empty());
  return max_interv;
}

crb_interval rb_helper::find_next_empty_interval(const crb_bitmap& used_crb_bitmap, crb_interval search_limits)
{
  return find_next_empty_interval_impl(used_crb_bitmap, search_limits);
}

crb_interval rb_helper::find_empty_interval_of_length(const crb_bitmap& used_crb_bitmap,
                                                      unsigned          nof_crbs,
                                                      crb_interval      search_limits)
{
  return find_empty_interval_of_length_impl(used_crb_bitmap, nof_crbs, search_limits);
}

prb_interval rb_helper::find_next_empty_interval(const prb_bitmap& used_prb_bitmap, prb_interval search_limits)
{
  return find_next_empty_interval_impl(used_prb_bitmap, search_limits);
}

prb_interval rb_helper::find_empty_interval_of_length(const prb_bitmap& used_prb_bitmap,
                                                      unsigned          nof_prbs,
                                                      prb_interval      search_limits)
{
  return find_empty_interval_of_length_impl(used_prb_bitmap, nof_prbs, search_limits);
}

vrb_interval rb_helper::find_next_empty_interval(const vrb_bitmap& used_vrb_bitmap, vrb_interval search_limits)
{
  return find_next_empty_interval_impl(used_vrb_bitmap, search_limits);
}

vrb_interval rb_helper::find_empty_interval_of_length(const vrb_bitmap& used_vrb_bitmap,
                                                      unsigned          nof_vrbs,
                                                      vrb_interval      search_limits)
{
  return find_empty_interval_of_length_impl(used_vrb_bitmap, nof_vrbs, search_limits);
}
