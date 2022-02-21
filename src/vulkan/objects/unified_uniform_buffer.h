/* Vulkan API helper objects. */

#pragma once

#include "buffer.h"
#include "common.h"

typedef struct vulkan_unified_uniform_buffer {
  /* CPU state  */
  struct {
    alignas(16) mat4 viewMat;
    alignas(16) mat4 projMat;
  } data; // HIRO move UBO definitions out of vulkan_unified_uniform_buffer

  /* GPU state */
  vulkan_buffer *buffer;

  bool dirty; /// True if CPU to GPU transfer required.
} vulkan_unified_uniform_buffer;

vulkan_unified_uniform_buffer *vulkan_unified_uniform_buffer_create(vulkan_device *vkd);
void vulkan_unified_uniform_buffer_destroy(vulkan_unified_uniform_buffer *uniformBuffer);

// HIRO: Unified uniform buffer. (register structs?)

void vulkan_unified_uniform_buffer_send_to_device(vulkan_unified_uniform_buffer *uniformBuffer);

void vulkan_unified_uniform_buffer_debug_print(vulkan_unified_uniform_buffer *uniformBuffer);
