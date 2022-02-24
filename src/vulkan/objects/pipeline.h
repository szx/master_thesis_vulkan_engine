/* Vulkan API helper objects. */

#pragma once

#include "../data/camera.h"
#include "../swap_chain.h"
#include "batch.h"
#include "common.h"
#include "render_cache_list.h"
#include "render_state.h"
#include "shader.h"

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

  // HIRO generate shaders from unified buffers.
  // HIRO generate *_uniform_descriptor_set in descriptor.h (layout + set)
  // HIRO match *_uniform_descriptor_set with shader reflect.
  UT_array *shaders; /// vulkan_shader* array.
  // HIRO vulkan_shader_dependencies? and create render pass using it

  VkDescriptorPool descriptorPool;
  /// VkDescriptorSetLayout array. One descriptor set layout per one shader binding (layout
  /// qualifier). VK_NULL_HANDLE if no shader binding for indexed set number.
  UT_array *descriptorSetLayouts;
  /// One descriptor set per one descriptor set layout.
  VkDescriptorSet descriptorSet;

  // HIRO: Generate render pass from shaders.
  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
} vulkan_pipeline;

vulkan_pipeline *vulkan_pipeline_create(vulkan_device *vkd, data_asset_db *assetDb,
                                        vulkan_batches *batches, vulkan_data_camera *camera,
                                        vulkan_unified_geometry_buffer *unifiedGeometryBuffer,
                                        vulkan_unified_uniform_buffer *unifiedUniformBuffer);
void vulkan_pipeline_destroy(vulkan_pipeline *pipeline);

void vulkan_pipeline_send_to_device(vulkan_pipeline *pipeline);

void vulkan_pipeline_debug_print(vulkan_pipeline *pipeline);
