/* Pipeline definitions.
 * First place to edit when adding new pipeline. */

#pragma once

#include "../common.h"

typedef struct vulkan_pipeline vulkan_pipeline;
typedef struct vulkan_pipeline_frame_state vulkan_pipeline_frame_state;

#define END_OF_VULKAN_PIPELINE_TYPES
#define VULKAN_PIPELINE_TYPES(X, ...)                                                              \
  X(forward, __VA_ARGS__)                                                                          \
  X(deferred_geometry, __VA_ARGS__)                                                                \
  X(deferred_lighting, __VA_ARGS__)                                                                \
  X(skybox, __VA_ARGS__)                                                                           \
  X(debug_text, __VA_ARGS__)                                                                       \
  END_OF_VULKAN_PIPELINE_TYPES

typedef enum vulkan_pipeline_type {
#define x(_name, ...) vulkan_pipeline_type_##_name,
  VULKAN_PIPELINE_TYPES(x, )
#undef x
      vulkan_pipeline_type_count
} vulkan_pipeline_type;

typedef enum vulkan_pipeline_offscreen_texture_type {
  vulkan_pipeline_offscreen_texture_type_g_buffer_0,
  vulkan_pipeline_offscreen_texture_type_g_buffer_1,
  vulkan_pipeline_offscreen_texture_type_g_buffer_2,
  vulkan_pipeline_offscreen_texture_type_count
} vulkan_pipeline_offscreen_texture_type;

typedef enum vulkan_pipeline_offscreen_texture_usage {
  vulkan_pipeline_offscreen_texture_usage_framebuffer_color_attachment,
  vulkan_pipeline_offscreen_texture_usage_fragment_shader_read,
  vulkan_pipeline_offscreen_texture_usage_count
} vulkan_pipeline_offscreen_texture_usage;

typedef struct vulkan_pipeline_offscreen_texture_info {
  vulkan_pipeline_offscreen_texture_type type;
  vulkan_pipeline_offscreen_texture_usage usage;
} vulkan_pipeline_offscreen_texture_info;

typedef struct vulkan_pipeline_info {
  vulkan_pipeline_type pipelineType;

  bool useOnscreenColorAttachment;
  VkClearColorValue onscreenClearValue;

  uint32_t offscreenTextureCount;
  vulkan_pipeline_offscreen_texture_info offscreenTextureInfos[MAX_OFFSCREEN_ATTACHMENT_COUNT];
  VkClearColorValue offscreenClearValue;

  bool useDepthAttachment;
  bool depthAttachmentWriteEnable;
  bool depthAttachmentTestEnable;
  VkCompareOp depthAttachmentTestOp;
  bool depthAttachmentLoadOp;
  bool depthAttachmentStoreOp;
  VkClearDepthStencilValue depthClearValue;

  bool colorBlendingType;
} vulkan_pipeline_info;

uint32_t vulkan_pipeline_info_get_framebuffer_attachment_count(vulkan_pipeline_info pipelineInfo);
uint32_t
vulkan_pipeline_info_get_framebuffer_color_attachment_count(vulkan_pipeline_info pipelineInfo);
uint32_t vulkan_pipeline_info_get_framebuffer_offscreen_color_attachment_count(
    vulkan_pipeline_info pipelineInfo);

void vulkan_pipeline_info_get_framebuffer_offscreen_attachment_image_views(
    vulkan_pipeline_info pipelineInfo, vulkan_pipeline_frame_state *frameState,
    VkImageView *offscreenImageViews);
VkImageView vulkan_pipeline_info_get_framebuffer_depth_attachment_image_view(
    vulkan_pipeline_info pipelineInfo, vulkan_pipeline_frame_state *frameState);
void vulkan_pipeline_info_get_framebuffer_offscreen_attachment_formats(
    vulkan_pipeline_info pipelineInfo, vulkan_pipeline_frame_state *frameState,
    VkFormat *offscreenFormats);
VkFormat vulkan_pipeline_info_get_framebuffer_depth_attachment_formats(
    vulkan_pipeline_info pipelineInfo, vulkan_pipeline_frame_state *frameState);

void vulkan_pipeline_info_get_framebuffer_attachment_image_views(
    vulkan_pipeline_info pipelineInfo, VkImageView swapChainImageView,
    VkImageView *offscreenImageViews, VkImageView depthBufferImageView,
    VkImageView *framebufferAttachments);

void vulkan_pipeline_info_get_framebuffer_attachment_clear_values(vulkan_pipeline_info pipelineInfo,
                                                                  VkClearValue *clearValues);

void vulkan_pipeline_info_get_render_pass_create_info(
    vulkan_pipeline_info pipelineInfo, vulkan_pipeline *prev, vulkan_pipeline *next,
    VkFormat swapChainImageFormat, VkFormat *offscreenImageFormats, VkFormat depthBufferImageFormat,
    VkAttachmentDescription *onscreenColorAttachmentDescription,
    VkAttachmentReference *onscreenColorAttachmentReference,
    VkAttachmentDescription *offscreenColorAttachmentDescriptions,
    VkAttachmentReference *offscreenColorAttachmentReferences,
    VkAttachmentDescription *depthAttachmentDescription,
    VkAttachmentReference *depthAttachmentReference);
