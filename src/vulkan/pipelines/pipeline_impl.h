/* Vulkan API helper objects. */

#pragma once

#include "../common.h"

#include "pipeline_defs.h"

#define VULKAN_PIPELINE_FUNCTIONS_DECLS(_name)                                                     \
  vulkan_pipeline_info vulkan_pipeline_impl_##_name##_get_pipeline_info(                           \
      vulkan_pipeline *pipeline);                                                                  \
                                                                                                   \
  void vulkan_pipeline_impl_##_name##_record_render_pass(                                          \
      vulkan_pipeline *pipeline, VkCommandBuffer commandBuffer, size_t swapChainImageIdx);

#define x(_name, ...) VULKAN_PIPELINE_FUNCTIONS_DECLS(_name)
VULKAN_PIPELINE_TYPES(x, )
#undef x

/* pipeline frame state */
