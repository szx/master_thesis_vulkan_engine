/* Vulkan API helper objects. */

#pragma once

#include "buffer.h"
#include "common.h"
#include "shader.h"

typedef struct vulkan_unified_uniform_buffer {
  /* CPU state */
  vulkan_instance_data_uniform_buffer_data *instanceData;
  /* GPU state */
  vulkan_buffer *buffer;

  bool dirty; /// True if CPU to GPU transfer required.
} vulkan_unified_uniform_buffer;

vulkan_unified_uniform_buffer *vulkan_unified_uniform_buffer_create(vulkan_device *vkd);
void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer);

/* HIRO: uniform/storage buffers for:
 * node cached materials
 */

void vulkan_unified_uniform_buffer_send_to_device(vulkan_unified_uniform_buffer *uniformBuffer);

void vulkan_unified_uniform_buffer_debug_print(vulkan_unified_uniform_buffer *uniformBuffer);
