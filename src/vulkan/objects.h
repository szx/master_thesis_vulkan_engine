/* Vulkan API helper objects. */

#pragma once

#include "device.h"
#include "functions.h"

typedef struct vulkan_scene_cache vulkan_scene_cache;

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

uint32_t vulkan_attribute_type_to_stride(vulkan_attribute_type vertexAttributes);

typedef struct vulkan_interleaved_vertex_stream_element {
  vec3 position;
  vec3 normal;
  vec3 color;
  vec2 texCoord;
} vulkan_interleaved_vertex_stream_element; // TODO: packed attribute

vulkan_interleaved_vertex_stream_element vulkan_interleaved_vertex_stream_element_default();

/// Interleaved vertex stream.
/// Generated from cache list.
typedef struct vulkan_interleaved_vertex_stream {
  vulkan_attribute_type attributes; /// Determines size of stream element.
  UT_array *indexData;              /// uint32_t array
  UT_array *vertexData;             /// vulkan_interleaved_vertex_stream_element array
} vulkan_interleaved_vertex_stream;

vulkan_interleaved_vertex_stream *
vulkan_interleaved_vertex_stream_create(vulkan_attribute_type attributes);
void vulkan_interleaved_vertex_stream_destroy(vulkan_interleaved_vertex_stream *stream);

void vulkan_interleaved_vertex_stream_add_primitive(vulkan_interleaved_vertex_stream *stream,
                                                    vulkan_scene_cache *cache);
void vulkan_interleaved_vertex_stream_add_stream(vulkan_interleaved_vertex_stream *stream,
                                                 vulkan_interleaved_vertex_stream *other);

// HIRO Unified uniform buffers
// HIRO support multiple buffers and descriptors (seperate for each frame)

/// Unified geometry buffer.
/// Used to aggregate scene's vertex data into one big vertex buffer.
typedef struct vulkan_unified_geometry_buffer {
  /* CPU state */
  vulkan_interleaved_vertex_stream *interleavedVertexStream;
  /* GPU state */
  vulkan_buffer *indexBuffer;
  vulkan_buffer *vertexBuffer;

  bool dirty; /// True if CPU to GPU transfer required.
} vulkan_unified_geometry_buffer;

vulkan_unified_geometry_buffer *vulkan_unified_geometry_buffer_create(vulkan_device *vkd);
void vulkan_unified_geometry_buffer_destroy(vulkan_unified_geometry_buffer *geometryBuffer);

void vulkan_unified_geometry_buffer_set_interleaved_vertex_stream(
    vulkan_unified_geometry_buffer *geometryBuffer, vulkan_interleaved_vertex_stream *stream);

void vulkan_unified_geometry_buffer_send_to_device(vulkan_unified_geometry_buffer *geometryBuffer);

void vulkan_unified_geometry_buffer_debug_print(vulkan_unified_geometry_buffer *geometryBuffer);

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
