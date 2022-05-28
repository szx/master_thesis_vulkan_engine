/* Pipeline definitions.
 * First place to edit when adding new renderPass. */

#pragma once

#include "../../common.h"

typedef struct vulkan_render_pass vulkan_render_pass;
typedef struct vulkan_render_pass_frame_state vulkan_render_pass_frame_state;

// NOTE: Start adding render pass here!
#define END_OF_VULKAN_RENDER_PASS_TYPES
#define VULKAN_RENDER_PASS_TYPES(X, ...)                                                           \
  X(forward, __VA_ARGS__)                                                                          \
  X(deferred_geometry, __VA_ARGS__)                                                                \
  X(deferred_lighting, __VA_ARGS__)                                                                \
  X(skybox, __VA_ARGS__)                                                                           \
  X(debug_text, __VA_ARGS__)                                                                       \
  END_OF_VULKAN_RENDER_PASS_TYPES

typedef enum vulkan_render_pass_type {
#define x(_name, ...) vulkan_render_pass_type_##_name,
  VULKAN_RENDER_PASS_TYPES(x, )
#undef x
      vulkan_render_pass_type_count
} vulkan_render_pass_type;

typedef struct vulkan_render_pass_offscreen_texture_info {
  vulkan_offscreen_texture_type type;
  vulkan_offscreen_texture_usage usage;
} vulkan_render_pass_offscreen_texture_info;

typedef struct vulkan_render_pass_info {
  vulkan_render_pass_type renderPassType;

  bool useOnscreenColorAttachment;
  VkClearColorValue onscreenClearValue;

  uint32_t offscreenTextureCount;
  vulkan_render_pass_offscreen_texture_info offscreenTextureInfos[MAX_RENDER_TARGET_COUNT];
  VkClearColorValue offscreenClearValue;

  bool useDepthAttachment;
  bool depthAttachmentWriteEnable;
  bool depthAttachmentTestEnable;
  VkCompareOp depthAttachmentTestOp;
  bool depthAttachmentLoadOp;
  bool depthAttachmentStoreOp;
  VkClearDepthStencilValue depthClearValue;

  bool colorBlendingType;
} vulkan_render_pass_info;

uint32_t
vulkan_render_pass_info_get_framebuffer_attachment_count(vulkan_render_pass_info renderPassInfo);
uint32_t vulkan_render_pass_info_get_framebuffer_color_attachment_count(
    vulkan_render_pass_info renderPassInfo);
uint32_t vulkan_render_pass_info_get_framebuffer_offscreen_color_attachment_count(
    vulkan_render_pass_info renderPassInfo);

void vulkan_render_pass_info_get_framebuffer_offscreen_attachment_image_views(
    vulkan_render_pass_info renderPassInfo, vulkan_render_pass_frame_state *frameState,
    VkImageView *offscreenImageViews);
VkImageView vulkan_render_pass_info_get_framebuffer_depth_attachment_image_view(
    vulkan_render_pass_info renderPassInfo, vulkan_render_pass_frame_state *frameState);
void vulkan_render_pass_info_get_framebuffer_offscreen_attachment_formats(
    vulkan_render_pass_info renderPassInfo, vulkan_render_pass_frame_state *frameState,
    VkFormat *offscreenFormats);
VkFormat vulkan_render_pass_info_get_framebuffer_depth_attachment_formats(
    vulkan_render_pass_info renderPassInfo, vulkan_render_pass_frame_state *frameState);

void vulkan_render_pass_info_get_framebuffer_attachment_image_views(
    vulkan_render_pass_info renderPassInfo, VkImageView swapChainImageView,
    VkImageView *offscreenImageViews, VkImageView depthBufferImageView,
    VkImageView *framebufferAttachments);

void vulkan_render_pass_info_get_framebuffer_attachment_clear_values(
    vulkan_render_pass_info renderPassInfo, VkClearValue *clearValues);

uint32_t vulkan_render_pass_info_get_dependency_count(vulkan_render_pass_info renderPassInfo);

void vulkan_render_pass_info_get_render_pass_create_info(
    vulkan_render_pass_info renderPassInfo, vulkan_render_pass *prev, vulkan_render_pass *next,
    VkFormat swapChainImageFormat, VkFormat *offscreenImageFormats, VkFormat depthBufferImageFormat,
    VkAttachmentDescription *onscreenColorAttachmentDescription,
    VkAttachmentReference *onscreenColorAttachmentReference,
    VkAttachmentDescription *offscreenColorAttachmentDescriptions,
    VkAttachmentReference *offscreenColorAttachmentReferences,
    VkAttachmentDescription *depthAttachmentDescription,
    VkAttachmentReference *depthAttachmentReference, VkSubpassDependency *dependencies);
