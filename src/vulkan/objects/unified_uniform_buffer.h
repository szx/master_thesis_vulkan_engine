/* Vulkan API helper objects. */

#pragma once

#include "buffer.h"
#include "common.h"
#include "render_cache_list.h"
#include "shader.h"

typedef struct vulkan_data_camera vulkan_data_camera;

typedef struct vulkan_unified_uniform_buffer {
  /* CPU state */
  vulkan_render_cache_list *renderCacheList; /// Pointer.
  vulkan_data_camera *camera;                /// Pointer.

  vulkan_instance_data_uniform_buffer_data *instanceData;
  vulkan_global_uniform_buffer_data *globalData;
  /* GPU state */
  vulkan_buffer *buffer;

  bool dirty; /// True if CPU to GPU transfer required.
} vulkan_unified_uniform_buffer;

vulkan_unified_uniform_buffer *
vulkan_unified_uniform_buffer_create(vulkan_device *vkd, vulkan_render_cache_list *renderCacheList,
                                     vulkan_data_camera *camera);
void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer);

void vulkan_unified_uniform_buffer_update(vulkan_unified_uniform_buffer *uniformBuffer);

/* HIRO: uniform/storage buffers for:
 * node cached materials
 */

void vulkan_unified_uniform_buffer_send_to_device(vulkan_unified_uniform_buffer *uniformBuffer);

void vulkan_unified_uniform_buffer_debug_print(vulkan_unified_uniform_buffer *uniformBuffer);
