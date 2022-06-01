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

typedef struct vulkan_offscreen_framebuffer_color_attachment_info {
  vulkan_image_type type;
  VkClearColorValue clearValue;
} vulkan_offscreen_framebuffer_color_attachment_info;

typedef struct vulkan_offscreen_fragment_shader_input_info {
  vulkan_image_type type;
} vulkan_offscreen_fragment_shader_input_info;

/// Describes render pass on high-level.
/// Used to create render pass and its corresponding render graph element.
typedef struct vulkan_render_pass_desc {
  bool useOnscreenColorAttachment;
  VkClearColorValue onscreenClearValue;

  uint32_t offscreenColorAttachmentCount;
  vulkan_offscreen_framebuffer_color_attachment_info
      offscreenColorAttachments[MAX_RENDER_TARGET_COUNT];

  uint32_t offscreenFragmentShaderInputCount;
  vulkan_offscreen_fragment_shader_input_info
      offscreenFragmentShaderInputs[MAX_RENDER_TARGET_COUNT];

  bool useDepthAttachment;
  bool depthAttachmentWriteEnable;
  bool depthAttachmentTestEnable;
  VkCompareOp depthAttachmentTestOp;
  VkClearDepthStencilValue depthClearValue;

  bool colorBlendingType;

  /* calculated additional info */
  vulkan_render_pass_type renderPassType;
  size_t attachmentCount;
  size_t colorAttachmentCount;
} vulkan_render_pass_desc;

void vulkan_render_pass_desc_calculate_additional_info(vulkan_render_pass_desc *renderPassDesc,
                                                       vulkan_render_pass *renderPass);
