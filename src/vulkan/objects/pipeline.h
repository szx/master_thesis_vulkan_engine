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
  /* CPU state */
  data_asset_db *assetDb;     ///< Pointer.
  vulkan_batches *batches;    ///< Pointer
  vulkan_data_camera *camera; ///< Pointer

  /* GPU state */
  vulkan_device *vkd;               ///< Pointer.
  vulkan_swap_chain *vks;           ///< Pointer.
  vulkan_render_state *renderState; ///< Pointer.

  vulkan_shader_generator *shaderGenerator;

  // HIRO generate descriptor sets from shader generator layouts.
  // HIRO move to render state? that can generate shaders easily
  VkDescriptorPool descriptorPool;
  /// One descriptor set per one descriptor set layout.
  VkDescriptorSet descriptorSet;

  // HIRO: Generate render pass from shaders.
  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
} vulkan_pipeline;

vulkan_pipeline *vulkan_pipeline_create(vulkan_swap_chain *vks, vulkan_render_state *renderState);
void vulkan_pipeline_destroy(vulkan_pipeline *pipeline);

void vulkan_pipeline_send_to_device(vulkan_pipeline *pipeline);

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline);
