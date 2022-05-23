/* Vulkan API helper objects. */

#pragma once

#include "../common.h"

#include "pipeline_defs.h"

typedef struct vulkan_pipeline_frame_state vulkan_pipeline_frame_state;

#define VULKAN_PIPELINE_FUNCTIONS_DECLS(_name)                                                     \
  vulkan_pipeline_info vulkan_pipeline_impl_##_name##_get_pipeline_info(                           \
      vulkan_pipeline *pipeline);                                                                  \
                                                                                                   \
  void vulkan_pipeline_impl_##_name##_record_render_pass(vulkan_pipeline *pipeline,                \
                                                         vulkan_pipeline_frame_state *frameState,  \
                                                         VkCommandBuffer commandBuffer);

#define x(_name, ...) VULKAN_PIPELINE_FUNCTIONS_DECLS(_name)
VULKAN_PIPELINE_TYPES(x, )
#undef x

/* pipeline frame state */
