/* Vulkan API helper objects. */

#pragma once

#include "render_pass_decls.h"

typedef struct vulkan_render_pass_frame_state vulkan_render_pass_frame_state;

#define VULKAN_RENDER_PASS_FUNCTIONS_DECLS(_name)                                                  \
  vulkan_render_pass_info vulkan_render_pass_impl_##_name##_get_info(                              \
      vulkan_render_pass *renderPass);                                                             \
                                                                                                   \
  void vulkan_render_pass_impl_##_name##_record_commands(                                          \
      vulkan_render_pass *renderPass, vulkan_render_pass_frame_state *frameState,                  \
      VkCommandBuffer commandBuffer);

#define x(_name, ...) VULKAN_RENDER_PASS_FUNCTIONS_DECLS(_name)
VULKAN_RENDER_PASS_TYPES(x, )
#undef x

/* renderPass frame state */
