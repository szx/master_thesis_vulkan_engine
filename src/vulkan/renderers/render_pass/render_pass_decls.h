/* Render pass high-level description. */

#pragma once

#include "../../common.h"

typedef struct vulkan_render_pass vulkan_render_pass;
typedef struct vulkan_render_pass_frame_state vulkan_render_pass_frame_state;

typedef struct vulkan_offscreen_framebuffer_color_attachment_info {
  vulkan_image_type type;
  VkClearColorValue clearValue;
} vulkan_offscreen_framebuffer_color_attachment_info;

typedef struct vulkan_offscreen_fragment_shader_input_info {
  vulkan_image_type type;
} vulkan_offscreen_fragment_shader_input_info;

typedef void (*vulkan_render_pass_desc_record_func)(vulkan_render_pass *renderPass,
                                                    vulkan_render_pass_frame_state *frameState,
                                                    VkCommandBuffer commandBuffer);

/// Describes render pass on high-level.
/// Used to create render pass and its corresponding render graph element.
typedef struct vulkan_render_pass_desc {
  const char *vertexShader;
  const char *fragmentShader;

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

  vulkan_render_pass_desc_record_func recordFunc;
  void *recordFuncData;

  /* calculated additional info */
  size_t attachmentCount;
  size_t colorAttachmentCount;
} vulkan_render_pass_desc;

void vulkan_render_pass_desc_calculate_additional_info(vulkan_render_pass_desc *renderPassDesc,
                                                       vulkan_render_pass *renderPass);
