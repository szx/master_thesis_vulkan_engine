/* Vulkan API helper objects. */

#pragma once

#include "../data/camera.h"
#include "../swap_chain.h"
#include "batch.h"
#include "common.h"
#include "render_cache_list.h"
#include "render_state.h"
#include "shader_generator.h"

/// Unified geometry buffer.
/// Used to aggregate scene's vertex data into one big vertex buffer.
typedef struct vulkan_pipeline {
  vulkan_swap_chain *vks;           ///< Pointer.
  vulkan_render_state *renderState; ///< Pointer.

  vulkan_shader_generator *shaderGenerator;

  // HIRO HIRO: Implement:
  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  UT_array *swapChainFramebuffers; ///< VkFramebuffer array.

} vulkan_pipeline;

vulkan_pipeline *vulkan_pipeline_create(vulkan_swap_chain *vks, vulkan_render_state *renderState);
void vulkan_pipeline_destroy(vulkan_pipeline *pipeline);

void vulkan_pipeline_record_command_buffer(vulkan_pipeline *pipeline, VkCommandBuffer commandBuffer,
                                           size_t inFlightImage);

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline);
