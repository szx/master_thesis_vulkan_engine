/* Pipeline definitions.
 * First place to edit when adding new pipeline. */

#pragma once

#include "../common.h"

typedef struct vulkan_pipeline vulkan_pipeline;
typedef struct vulkan_pipeline_frame_state vulkan_pipeline_frame_state;

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

typedef enum vulkan_pipeline_offscreen_attachment_type {
  vulkan_pipeline_offscreen_attachment_type_gbuffer_color,
  vulkan_pipeline_offscreen_attachment_type_count
} vulkan_pipeline_offscreen_attachment_type;

typedef struct vulkan_pipeline_info {
  bool useOnscreenColorAttachment;
  VkClearColorValue onscreenClearValue;

  uint32_t offscreenColorAttachmentCount;
  vulkan_pipeline_offscreen_attachment_type
      offscreenColorAttachmentTypes[MAX_OFFSCREEN_ATTACHMENT_COUNT];
  VkClearColorValue offscreenClearValue;

  bool useDepthAttachment;
  VkClearDepthStencilValue depthClearValue;
} vulkan_pipeline_info;

uint32_t vulkan_pipeline_info_get_framebuffer_attachment_count(vulkan_pipeline_info pipelineInfo);

void vulkan_pipeline_info_get_framebuffer_attachment_image_views(
    vulkan_pipeline_info pipelineInfo, VkImageView swapChainImageView,
    VkImageView *offscreenImageViews, VkImageView depthBufferImageView,
    VkImageView *framebufferAttachments);

void vulkan_pipeline_info_get_framebuffer_attachment_clear_values(vulkan_pipeline_info pipelineInfo,
                                                                  VkClearValue *clearValues);

void vulkan_pipeline_info_get_render_pass_create_info(
    vulkan_pipeline_info pipelineInfo, vulkan_pipeline *prev, vulkan_pipeline *next,
    VkFormat swapChainImageFormat, VkFormat depthBufferImageFormat,
    VkAttachmentDescription *onscreenColorAttachmentDescription,
    VkAttachmentReference *onscreenColorAttachmentReference,
    VkAttachmentDescription *offscreenColorAttachmentDescriptions,
    VkAttachmentReference *offscreenColorAttachmentReferences,
    VkAttachmentDescription *depthAttachmentDescription,
    VkAttachmentReference *depthAttachmentReference);

/* pipeline frame state */

#include "../renderers/batch.h"

typedef struct vulkan_pipeline_impl_forward_frame_state {
  // HIRO Maintain two batches for transparent and opaque objects.
  vulkan_draw_call_data rendererCacheBatchesData;
} vulkan_pipeline_impl_forward_frame_state;

typedef struct vulkan_pipeline_impl_skybox_frame_state {
  void *skyboxFrameStateIsEmptyForNow;
} vulkan_pipeline_impl_skybox_frame_state;
