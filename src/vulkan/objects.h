/* Vulkan API helper objects. */

#pragma once

#include "device.h"
#include "functions.h"

// HIRO vulkan_buffer
// HIRO vertex interleaving in vulkan_geometry_buffer

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

// HIRO rename to objects
// HIRO basic log-level wrappers: buffer
// HIRO basic high-level wrappers: vertex element, vertex buffer, uniform/storage buffer for shader

// Geometry buffer.
// Used to aggregate scene's vertex data into device local buffers.
typedef struct vulkan_geometry_buffer {
  /* CPU state */
  UT_array *data; /// uint8_t
  /* GPU state */
  vulkan_device *vkd; /// vulkan_device pointer
  VkBuffer buffer;
  VkDeviceMemory bufferMemory;

  bool dirty; /// True if CPU to GPU transfer required.
} vulkan_geometry_buffer;

typedef struct vulkan_uniform_buffer {
  /* initialized by vulkan_uniform_buffer_create */
  vulkan_device *vkd; /// vulkan_device pointer
  VkBuffer buffer;
  VkDeviceMemory bufferMemory;
  VkDeviceSize bufferMemorySize;
  /* initialized and updated by vulkan_uniform_buffer_send_to_device */
  struct {
    alignas(16) mat4 viewMat;
    alignas(16) mat4 projMat;
  } data; // HIRO move UBO definitions out of vulkan_uniform_buffer

  bool dirty;            /// True if uniform buffer data updated on CPU.
} vulkan_uniform_buffer; // TODO: Manage every uniform buffers.

vulkan_geometry_buffer *vulkan_geometry_buffer_create();
void vulkan_geometry_buffer_destroy(vulkan_geometry_buffer *geometryBuffer);
/// Creates device local buffer and copies geometry buffer data into it.
void vulkan_geometry_buffer_send_to_device(vulkan_device *vkd,
                                           vulkan_geometry_buffer *geometryBuffer);

/// Creates host visible and coherent buffer.
/// Descriptor set is created and managed by pipeline.
/// NOTE: We do not use device local memory, because we assume that mapping host visible and
///       coherent memory is faster than copying into device local memory using staging buffer.
vulkan_uniform_buffer *vulkan_uniform_buffer_create(vulkan_device *vkd);
void vulkan_uniform_buffer_destroy(vulkan_uniform_buffer *uniformBuffer);
/// Maps and updates uniform buffer memory.
void vulkan_uniform_buffer_send_to_device(vulkan_uniform_buffer *uniformBuffer);
