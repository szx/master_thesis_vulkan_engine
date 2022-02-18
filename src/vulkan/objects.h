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

typedef struct vulkan_interleaved_vertex_stream_element {
  vec3 position;
  vec3 normal;
  vec3 color;
  vec2 texCoord;
} vulkan_interleaved_vertex_stream_element;

/// Interleaved vertex stream.
typedef struct vulkan_interleaved_vertex_stream {
  vulkan_attribute_type attributes;
  UT_array *data; /// vulkan_interleaved_vertex_stream_element list
} vulkan_interleaved_vertex_stream;

vulkan_interleaved_vertex_stream *
vulkan_interleaved_vertex_stream_create(vulkan_attribute_type attributes);
// HIRO add primitive, add stream
void vulkan_interleaved_vertex_stream_destroy(vulkan_interleaved_vertex_stream *stream);

uint32_t vertex_types_to_vertex_stride(vulkan_attribute_type vertexAttributes);
uint32_t index_type_to_index_stride(vulkan_index_type indexType);
vulkan_index_type index_stride_to_index_type(uint32_t indexStride);
VkIndexType stride_to_index_format(uint32_t indexStride);

// HIRO Unified uniform buffers
// HIRO support multiple buffers and descriptors (seperate for each frame)

/// Element of an unified buffer.
/// Corresponds to one contiguous memory block in both CPU and GPU buffer.
typedef struct vulkan_unified_buffer_element {
  void **data; /// Pointer to pointer to memory block.
  size_t size; /// Memory block size.

  bool dirty; // True if CPU to GPU transfer needed.
  struct vulkan_unified_buffer_element *prev, *next;
} vulkan_unified_buffer_element;

vulkan_unified_buffer_element *vulkan_unified_buffer_element_create(void **data, size_t size);
void vulkan_unified_buffer_element_destroy(vulkan_unified_buffer_element *element);

/// Unified geometry buffer.
/// Used to aggregate scene's vertex data into one big vertex buffer.
typedef struct vulkan_unified_geometry_buffer {
  /* CPU state */
  vulkan_unified_buffer_element *elements; /// List of memory blocks with vertex data.
  /* GPU state */
  vulkan_buffer *buffer;

  bool dirty; /// True if CPU to GPU transfer required.
} vulkan_unified_geometry_buffer;

vulkan_unified_geometry_buffer *vulkan_unified_geometry_buffer_create(vulkan_device *vkd);
void vulkan_unified_geometry_buffer_destroy(vulkan_unified_geometry_buffer *geometryBuffer);

void vulkan_unified_geometry_buffer_add_element(vulkan_unified_geometry_buffer *geometryBuffer,
                                                vulkan_unified_buffer_element *element);

// HIRO vertex interleaving in scene renderer.

void vulkan_unified_geometry_buffer_send_to_device(vulkan_device *vkd,
                                                   vulkan_unified_geometry_buffer *geometryBuffer);

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
