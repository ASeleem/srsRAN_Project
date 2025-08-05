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

#include "ofh_data_flow_cplane_scheduling_commands.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/rtsan.h"
#include <memory>

namespace srsran {
namespace ofh {

/// Open Fronthaul Control-Plane scheduling and beamforming commands data flow task dispatcher implementation.
class data_flow_cplane_downlink_task_dispatcher : public data_flow_cplane_scheduling_commands
{
public:
  data_flow_cplane_downlink_task_dispatcher(srslog::basic_logger&                                 logger_,
                                            std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane_,
                                            task_executor&                                        executor_,
                                            unsigned                                              sector_id_) :
    logger(logger_), data_flow_cplane(std::move(data_flow_cplane_)), executor(executor_), sector_id(sector_id_)
  {
    srsran_assert(data_flow_cplane, "Invalid data flow");
  }

  // See interface for documentation.
  void enqueue_section_type_1_message(const data_flow_cplane_type_1_context& context) override
  {
    if (!executor.execute([this, context]() noexcept SRSRAN_RTSAN_NONBLOCKING {
          data_flow_cplane->enqueue_section_type_1_message(context);
        })) {
      logger.warning(
          "Sector#{}: failed to dispatch Control-Plane type 1 message for slot '{}'", sector_id, context.slot);
    }
  }

  // See interface for documentation.
  void enqueue_section_type_3_prach_message(const data_flow_cplane_scheduling_prach_context& context) override
  {
    if (!executor.execute([this, context]() noexcept SRSRAN_RTSAN_NONBLOCKING {
          data_flow_cplane->enqueue_section_type_3_prach_message(context);
        })) {
      logger.warning(
          "Sector#{}: failed to dispatch Control-Plane type 3 message for slot '{}'", sector_id, context.slot);
    }
  }

  // See interface for documentation.
  data_flow_message_encoding_metrics_collector* get_metrics_collector() override
  {
    return data_flow_cplane->get_metrics_collector();
  }

private:
  srslog::basic_logger&                                 logger;
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane;
  task_executor&                                        executor;
  const unsigned                                        sector_id;
};

} // namespace ofh
} // namespace srsran
