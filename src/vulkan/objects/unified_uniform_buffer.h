/* Vulkan API helper objects. */

#pragma once

#include "buffer.h"
#include "render_cache_list.h"
#include "shader.h"

typedef struct vulkan_data_camera vulkan_data_camera;

typedef struct vulkan_unified_uniform_buffer {
  /* CPU state */
  vulkan_render_cache_list *renderCacheList; /// Pointer.

  vulkan_instances_uniform_buffer_data *instancesData;
  vulkan_global_uniform_buffer_data *globalData;

  /* GPU state */
  vulkan_buffer *buffer;
  vulkan_descriptors *descriptors;

} vulkan_unified_uniform_buffer;

vulkan_unified_uniform_buffer *
vulkan_unified_uniform_buffer_create(vulkan_device *vkd, vulkan_render_cache_list *renderCacheList);
void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer);

void vulkan_unified_uniform_buffer_update(vulkan_unified_uniform_buffer *uniformBuffer,
                                          vulkan_data_camera *camera);

/* HIRO: uniform/storage buffers for:
 * node cached materials
 */

void vulkan_unified_uniform_buffer_send_to_device(vulkan_unified_uniform_buffer *uniformBuffer);

void vulkan_unified_uniform_buffer_record_bind_command(vulkan_unified_uniform_buffer *uniformBuffer,
                                                       VkCommandBuffer commandBuffer,
                                                       VkPipelineLayout pipelineLayout);

void vulkan_unified_uniform_buffer_debug_print(vulkan_unified_uniform_buffer *uniformBuffer);
