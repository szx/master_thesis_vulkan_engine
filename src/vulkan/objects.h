/* Vulkan API helper objects. */

#pragma once

#include "device.h"
#include "functions.h"

typedef enum vulkan_buffer_type {
  vulkan_buffer_type_geometry,
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
  VkBuffer buffer;
  VkDeviceMemory bufferMemory;

  bool resident; /// True if buffer created in GPU memory.
  bool dirty;    /// True if CPU to GPU transfer required.
} vulkan_buffer;

vulkan_buffer *vulkan_buffer_create(vulkan_device *vkd, vulkan_buffer_type type, const void *data,
                                    size_t size);
void vulkan_buffer_destroy(vulkan_buffer *buffer);

void vulkan_buffer_send_to_device(vulkan_buffer *buffer);

/// One element of vertex stream.
/// Assumes that all vertex attributes are interleaved.
/// If vertex attribute is not used, we have to do one of following:
///     - adjust vertex binding stride and shader (if it is after all other used vertex attributes)
///     - fill vertex attribute with default value
typedef struct vulkan_vertex_stream_element {
  vec3 position;
  vec3 normal;
  vec3 color;
  vec2 texCoord;
} vulkan_vertex_stream_element; // HIRO create interleaved vertex stream when building scene
// TODO: Packing.

typedef enum vulkan_attribute_type {
  UnknownAttribute = 1 << 0, // TODO naming
  PositionAttribute = 1 << 1,
  NormalAttribute = 1 << 2,
  ColorAttribute = 1 << 3,
  TexCoordAttribute = 1 << 4,
  TangentAttribute = 1 << 5,
} vulkan_attribute_type;

typedef enum vulkan_index_type {
  vulkan_index_type_unknown = 0,
  vulkan_index_type_uint32 = 1
} vulkan_index_type;

uint32_t vertex_types_to_vertex_stride(vulkan_attribute_type vertexAttributes);
uint32_t index_type_to_index_stride(vulkan_index_type indexType);
vulkan_index_type index_stride_to_index_type(uint32_t indexStride);
VkIndexType stride_to_index_format(uint32_t indexStride);

// HIRO Unified buffers
// HIRO support multiple buffers and descriptors (seperate for each frame)

// Geometry buffer.
// Used to aggregate scene's vertex data into device local buffers.
typedef struct vulkan_geometry_buffer {
  /* CPU state */
  UT_array *data; /// uint8_t
  /* GPU state */
  vulkan_buffer *buffer;

  bool dirty; /// True if CPU to GPU transfer required.
} vulkan_geometry_buffer;

vulkan_geometry_buffer *vulkan_geometry_buffer_create(vulkan_device *vkd);
void vulkan_geometry_buffer_destroy(vulkan_geometry_buffer *geometryBuffer);

// HIRO vulkan_unified_geometry_buffer: adds UT_array to geometry buffer
// HIRO vertex interleaving in vulkan_geometry_buffer

void vulkan_geometry_buffer_send_to_device(vulkan_device *vkd,
                                           vulkan_geometry_buffer *geometryBuffer);

typedef struct vulkan_uniform_buffer {
  /* CPU state  */
  struct {
    alignas(16) mat4 viewMat;
    alignas(16) mat4 projMat;
  } data; // HIRO move UBO definitions out of vulkan_uniform_buffer

  /* GPU state */
  vulkan_buffer *buffer;

  bool dirty; /// True if CPU to GPU transfer required.
} vulkan_uniform_buffer;

vulkan_uniform_buffer *vulkan_uniform_buffer_create(vulkan_device *vkd);
void vulkan_uniform_buffer_destroy(vulkan_uniform_buffer *uniformBuffer);

// HIRO: Unified uniform buffer. (register structs?)

void vulkan_uniform_buffer_send_to_device(vulkan_uniform_buffer *uniformBuffer);
