/* Vulkan API helper objects. */

#pragma once

#include "common.h"

typedef enum vulkan_buffer_type {
  vulkan_buffer_type_geometry_index,
  vulkan_buffer_type_geometry_vertex,
  vulkan_buffer_type_uniform,
  vulkan_buffer_type_count
} vulkan_buffer_type;

/// Buffer.
/// Used to store data in memory.
typedef struct vulkan_buffer {
  /* CPU state */
  const void *data; /// Block of memory on CPU.
  size_t size;      /// Size of data in bytes.

  /* GPU state */
  vulkan_device *vkd; /// Pointer.
  vulkan_buffer_type type;
  VkBufferUsageFlags bufferUsageFlags;
  VkMemoryPropertyFlags memoryPropertyFlags;
  const char *name;
  VkBuffer buffer;
  VkDeviceMemory bufferMemory;

  bool resident; /// True if buffer created in GPU memory.
  bool dirty;    /// True if CPU to GPU transfer required.
} vulkan_buffer;

vulkan_buffer *vulkan_buffer_create(vulkan_device *vkd, vulkan_buffer_type type, const void *data,
                                    size_t size);
void vulkan_buffer_destroy(vulkan_buffer *buffer);

void vulkan_buffer_make_resident(vulkan_buffer *buffer);
void vulkan_buffer_send_to_device(vulkan_buffer *buffer);
