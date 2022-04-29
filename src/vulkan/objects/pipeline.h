/* Vulkan API helper objects. */

#pragma once

#include "../data/camera.h"
#include "batch.h"
#include "image.h"
#include "pipeline_state.h"
#include "render_cache_list.h"
#include "render_state.h"
#include "shader_program.h"
#include "swap_chain.h"

/// Unified geometry buffer.
/// Used to aggregate scene's vertex data into one big vertex buffer.
typedef struct vulkan_pipeline {
  vulkan_swap_chain *vks;                            ///< Pointer.
  vulkan_render_state *renderState;                  ///< Pointer.
  vulkan_pipeline_shared_state *pipelineSharedState; ///< Pointer.

  vulkan_shader_program *shaderProgram;
  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  UT_array *frameStates; ///< vulkan_pipeline_frame_state array.
} vulkan_pipeline;

vulkan_pipeline *vulkan_pipeline_create(vulkan_swap_chain *vks, vulkan_render_state *renderState,
                                        vulkan_pipeline_shared_state *pipelineSharedState);
void vulkan_pipeline_destroy(vulkan_pipeline *pipeline);

void vulkan_pipeline_init(vulkan_pipeline *pipeline, vulkan_swap_chain *vks,
                          vulkan_render_state *renderState,
                          vulkan_pipeline_shared_state *pipelineSharedState);
void vulkan_pipeline_deinit(vulkan_pipeline *pipeline);

size_t vulkan_pipeline_get_framebuffer_attachment_count(vulkan_pipeline *pipeline);
void vulkan_pipeline_get_framebuffer_attachments(vulkan_pipeline *pipeline,
                                                 size_t swapChainImageIdx,
                                                 VkImageView *attachments);
void vulkan_pipeline_get_framebuffer_attachment_clear_values(vulkan_pipeline *pipeline,
                                                             VkClearValue *clearValues);

void vulkan_pipeline_send_to_device(vulkan_pipeline *pipeline, size_t swapChainImageIdx);
void vulkan_pipeline_record_render_pass(vulkan_pipeline *pipeline, VkCommandBuffer commandBuffer,
                                        size_t swapChainImageIdx);

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline);
