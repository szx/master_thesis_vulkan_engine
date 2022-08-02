/* Vulkan API helper objects. */

#pragma once

#include "../common.h"

typedef struct buffer buffer;

typedef enum buffer_type {
  buffer_type_geometry_index,
  buffer_type_geometry_vertex,
  buffer_type_uniform,
  buffer_type_indirect_draw,
  buffer_type_count
} buffer_type;

/// One continuous memory block that makes up buffer.
typedef struct buffer_element {
  buffer *buffer;        ///< Pointer.
  const void *data;      ///< Memory block on CPU.
  size_t size;           ///< Size of memory block on CPU in bytes.
  uint32_t bufferOffset; ///< Offset in buffer.
} buffer_element;

void buffer_element_update(buffer_element *bufferElement);
void buffer_element_debug_print(buffer_element *bufferElement, int indent);

/// Buffer.
/// Used to store data in memory.
/// NOTE: Buffer size can't change if resident.
typedef struct buffer {
  /* CPU state */
  size_t totalSize;   ///< Size of buffer.
  UT_array *elements; ///< buffer_element list.

  /* GPU state */
  device *vkd; ///< Pointer.
  buffer_type type;
  VkBufferUsageFlags bufferUsageFlags;
  VkMemoryPropertyFlags memoryPropertyFlags;
  const char *name;
  VkBuffer buffer;
  VkDeviceMemory bufferMemory;

  bool resident; ///< True if buffer created in GPU memory.
  bool dirty;    ///< True if CPU to GPU transfer required.
} buffer;

buffer *buffer_create(device *vkd, buffer_type type);
void buffer_destroy(buffer *buffer);

buffer_element buffer_add(buffer *buffer, const void *data, size_t size);
void buffer_make_resident(buffer *buffer);
void buffer_send_to_device(buffer *buffer);
