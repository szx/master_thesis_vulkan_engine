/* Vulkan API helper objects. */

#pragma once

#include "../data/camera.h"
#include "../swap_chain.h"
#include "batch.h"
#include "common.h"
#include "image.h"
#include "render_cache_list.h"
#include "render_state.h"
#include "shader_generator.h"

typedef struct vulkan_pipeline_frame_state vulkan_pipeline_frame_state;
typedef struct vulkan_pipeline_shared_state vulkan_pipeline_shared_state;

/// Unified geometry buffer.
/// Used to aggregate scene's vertex data into one big vertex buffer.
typedef struct vulkan_pipeline {
  vulkan_swap_chain *vks;           ///< Pointer.
  vulkan_render_state *renderState; ///< Pointer.

  vulkan_shader_generator *shaderGenerator;
  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  UT_array *frameStates; ///< vulkan_pipeline_frame_state array.
  vulkan_pipeline_shared_state *sharedState;
} vulkan_pipeline;

vulkan_pipeline *vulkan_pipeline_create(vulkan_swap_chain *vks, vulkan_render_state *renderState);
void vulkan_pipeline_destroy(vulkan_pipeline *pipeline);

size_t vulkan_pipeline_get_framebuffer_attachment_count(vulkan_pipeline *pipeline);
void vulkan_pipeline_get_framebuffer_attachments(vulkan_pipeline *pipeline,
                                                 size_t swapChainImageIdx,
                                                 VkImageView *attachments);

void vulkan_pipeline_record_command_buffer(vulkan_pipeline *pipeline, VkCommandBuffer commandBuffer,
                                           size_t swapChainImageIndex);

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline);

/// Manages frame-specific resources (command buffer, framebuffer) used by pipeline to render one
/// one swap chain frame.
typedef struct vulkan_pipeline_frame_state {
  vulkan_pipeline *pipeline; ///< Pointer.

  VkFramebuffer swapChainFramebuffer;
  uint32_t swapChainImageIdx; ///< Index in vks->swapChainImages associated with framebuffer.

  VkCommandPool commandPool;
  VkCommandBuffer commandBuffer;
} vulkan_pipeline_frame_state;

void vulkan_pipeline_frame_state_init(vulkan_pipeline_frame_state *frameState,
                                      vulkan_pipeline *pipeline, uint32_t swapChainImageIdx);
void vulkan_pipeline_frame_state_deinit(vulkan_pipeline_frame_state *frameState);

void vulkan_pipeline_frame_state_debug_print(vulkan_pipeline_frame_state *frameState, int indent);

/// Manages resources shared between frames (depth buffer) used by pipeline to render one
/// one swap chain frame.
typedef struct vulkan_pipeline_shared_state {
  vulkan_pipeline *pipeline; ///< Pointer.

  vulkan_image *depthBufferImage;
} vulkan_pipeline_shared_state;

vulkan_pipeline_shared_state *vulkan_pipeline_shared_state_create(vulkan_pipeline *pipeline);
void vulkan_pipeline_shared_state_destroy(vulkan_pipeline_shared_state *sharedState);

void vulkan_pipeline_shared_state_debug_print(vulkan_pipeline_shared_state *sharedState,
                                              int indent);
