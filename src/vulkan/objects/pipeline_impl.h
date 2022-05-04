/* Vulkan API helper objects. */

#pragma once

#include "../data/camera.h"
#include "batch.h"
#include "image.h"
#include "pipeline_state.h"
#include "render_cache_list.h"
#include "render_state.h"
#include "swap_chain.h"

#define END_OF_VULKAN_PIPELINE_TYPES
#define VULKAN_PIPELINE_TYPES(X, ...)                                                              \
  X(forward, __VA_ARGS__)                                                                          \
  X(skybox, __VA_ARGS__)                                                                           \
  END_OF_VULKAN_PIPELINE_TYPES

typedef enum vulkan_pipeline_type {
#define x(_name, ...) vulkan_pipeline_type_##_name,
  VULKAN_PIPELINE_TYPES(x, )
#undef x
      vulkan_pipeline_type_count
} vulkan_pipeline_type;

typedef struct vulkan_pipeline vulkan_pipeline;

#define VULKAN_PIPELINE_FUNCTIONS_DECLS(_name)                                                     \
  void vulkan_pipeline_impl_##_name##_get_framebuffer_attachment_count(                            \
      vulkan_pipeline *pipeline, uint32_t *colorAttachmentCount, bool *useDepthAttachment);        \
                                                                                                   \
  void vulkan_pipeline_impl_##_name##_get_framebuffer_attachment_image_views(                      \
      vulkan_pipeline *pipeline, size_t swapChainImageIdx, VkImageView *attachments);              \
                                                                                                   \
  void vulkan_pipeline_impl_##_name##_get_framebuffer_attachment_clear_values(                     \
      vulkan_pipeline *pipeline, VkClearValue *clearValues);                                       \
                                                                                                   \
  void vulkan_pipeline_impl_##_name##_get_render_pass_create_info(                                 \
      vulkan_pipeline *pipeline, VkAttachmentDescription *colorAttachmentDescriptions,             \
      VkAttachmentReference *colorAttachmentReferences,                                            \
      VkAttachmentDescription *depthAttachmentDescription,                                         \
      VkAttachmentReference *depthAttachmentReference);                                            \
                                                                                                   \
  void vulkan_pipeline_impl_##_name##_record_render_pass(                                          \
      vulkan_pipeline *pipeline, VkCommandBuffer commandBuffer, size_t swapChainImageIdx);

#define x(_name, ...) VULKAN_PIPELINE_FUNCTIONS_DECLS(_name)
VULKAN_PIPELINE_TYPES(x, )
#undef x
