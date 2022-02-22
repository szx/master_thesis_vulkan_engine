/* Vulkan API helper objects. */

#pragma once

#include "common.h"

typedef enum vulkan_buffer_type {
  vulkan_buffer_type_geometry_index,
  vulkan_buffer_type_geometry_vertex,
  vulkan_buffer_type_uniform,
  vulkan_buffer_type_count
} vulkan_buffer_type;

/// One continuous memory block that makes up vulkan_buffer.
typedef struct vulkan_buffer_element {
  const void *data;      /// Memory block on CPU.
  size_t size;           /// Size of memory block on CPU in bytes.
  uint32_t bufferOffset; /// Offset in vulkan_buffer.
} vulkan_buffer_element;

/// Buffer.
/// Used to store data in memory.
/// NOTE: Buffer size can't change if resident.
typedef struct vulkan_buffer {
  /* CPU state */
  size_t totalSize;   /// Size of buffer.
  UT_array *elements; /// vulkan_buffer_element list.

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

vulkan_buffer *vulkan_buffer_create(vulkan_device *vkd, vulkan_buffer_type type);
void vulkan_buffer_destroy(vulkan_buffer *buffer);

vulkan_buffer_element vulkan_buffer_add(vulkan_buffer *buffer, const void *data, size_t size);
void vulkan_buffer_make_resident(vulkan_buffer *buffer);
void vulkan_buffer_send_to_device(vulkan_buffer *buffer);
