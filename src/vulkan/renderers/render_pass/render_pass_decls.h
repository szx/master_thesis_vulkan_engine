/* Render pass high-level description. */

#pragma once

#include "../../common.h"

typedef struct render_pass render_pass;
typedef struct render_pass_frame_state render_pass_frame_state;

typedef struct offscreen_framebuffer_color_attachment_info {
  const char *name;
  VkClearColorValue clearValue;
} offscreen_framebuffer_color_attachment_info;

typedef struct offscreen_fragment_shader_input_info {
  const char *name;
  uint32_t _offscreenTextureIdx;
} offscreen_fragment_shader_input_info;

typedef struct offscreen_framebuffer_depth_attachment_info {
  const char *name;
  bool depthWriteEnable;
  bool depthTestEnable;
  VkCompareOp depthTestOp;
  VkClearDepthStencilValue clearValue;
} offscreen_framebuffer_depth_attachment_info;

typedef void (*render_pass_desc_record_func)(render_pass *renderPass,
                                             render_pass_frame_state *frameState,
                                             VkCommandBuffer commandBuffer);

/// Describes render pass on high-level.
/// Used to create render pass and its corresponding render graph element.
typedef struct render_pass_desc {
  const char *vertexShader;
  const char *fragmentShader;
  bool forceEarlyFragmentTests;

  bool useOnscreenColorAttachment;
  VkClearColorValue onscreenClearValue;

  uint32_t offscreenColorAttachmentCount;
  offscreen_framebuffer_color_attachment_info offscreenColorAttachments[MAX_RENDER_TARGET_COUNT];

  uint32_t offscreenFragmentShaderInputCount;
  offscreen_fragment_shader_input_info offscreenFragmentShaderInputs[MAX_RENDER_TARGET_COUNT];

  offscreen_framebuffer_depth_attachment_info offscreenDepthAttachment;

  bool colorBlendingType;

  render_pass_desc_record_func recordFunc;

  /* calculated additional info */
  size_t attachmentCount;
  size_t colorAttachmentCount;
} render_pass_desc;

void render_pass_desc_calculate_additional_info(render_pass_desc *renderPassDesc);
