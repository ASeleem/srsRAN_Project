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

#include <cstdint>
#include <optional>
#include <string>
#include <variant>

namespace srsran {

struct ecef_coordinates_t {
  int position_x;
  int position_y;
  int position_z;
  int velocity_vx;
  int velocity_vy;
  int velocity_vz;
};

struct orbital_coordinates_t {
  uint64_t semi_major_axis;
  double   eccentricity;
  double   periapsis;
  double   longitude;
  double   mean_anomaly;
  double   inclination;
};

struct ta_info_t {
  unsigned ta_common;
  int      ta_common_drift;
  unsigned ta_common_drift_variant;
};

struct epoch_time_t {
  unsigned sfn;
  unsigned subframe_number;
};

struct ntn_config {
  /// SIB 19 values
  /// Reference location of the serving cell provided via NTN quasi-Earth fixed system. (TS 38.304)
  std::optional<std::string> reference_location;
  /// Distance from the serving cell reference location, as defined in TS 38.304. Each step represents 50m.
  std::optional<unsigned> distance_threshold;
  // NTN-config values
  /// Indicate the epoch time for the NTN assistance information.
  std::optional<epoch_time_t> epoch_time;
  /// Scheduling offset used for the timing relationships that are modified for NTN (see TS 38.213). The unit of the
  /// field K_offset is number of slots for a given subcarrier spacing of 15 kHz.
  unsigned cell_specific_koffset;
  /// Scheduling offset provided by network if downlink and uplink frame timing are not aligned at gNB.
  std::optional<unsigned> k_mac;
  /// A validity duration configured by the network for assistance information which indicates the maximum time duration
  /// (from epochTime) during which the UE can apply assistance information without having acquired new assistance
  /// information. Values {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 120, 180, 240, 900} seconds.
  std::optional<unsigned> ntn_ul_sync_validity_dur;
  /// This field provides satellite ephemeris either in format of position and velocity state vector or in format of
  /// orbital parameters.
  std::variant<ecef_coordinates_t, orbital_coordinates_t> ephemeris_info;
  /// Network-controlled common timing advanced value and it may include any timing offset considered necessary by the
  /// network.
  std::optional<ta_info_t> ta_info;
  /// SIB19 scheduling information.
  unsigned                si_msg_idx;
  unsigned                si_period_rf;
  unsigned                si_window_len_slots;
  std::optional<unsigned> si_window_position;
};
} // namespace srsran
