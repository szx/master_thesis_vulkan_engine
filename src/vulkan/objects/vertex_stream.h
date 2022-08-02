/* Vulkan API helper objects. */

#pragma once

#include "../common.h"

uint32_t vertex_attribute_type_to_stride(vertex_attribute_type vertexAttributes);

typedef struct vertex {
  vec3 position;
  vec3 normal;
  vec3 color;
  vec2 texCoord;
  vec4 tangent;
} vertex;

vertex vertex_default();

typedef struct vertex_stream_element {
  vertex_attribute_type attributes;
  uint32_t firstIndexOffset;
  uint32_t firstVertexOffset;
} vertex_stream_element;

/// Interleaved vertex stream.
typedef struct vertex_stream {
  vertex_attribute_type attributes; ///< Vertex attributes that can be present in vertex stream.
  UT_array *indexData;              ///< uint32_t array
  UT_array *vertexData;             ///< vertex array

  /// True in new geometry has been added.
  /// Unified geometry buffer is responsible for synchronizing vertex stream.
  bool dirty;
} vertex_stream;

vertex_stream *vertex_stream_create(vertex_attribute_type attributes);

void vertex_stream_update(vertex_stream *stream);

void vertex_stream_destroy(vertex_stream *stream);

vertex_stream_element vertex_stream_add_geometry(vertex_stream *stream, uint32_t vertexCount,
                                                 UT_array *indices, UT_array *positions,
                                                 UT_array *normals, UT_array *colors,
                                                 UT_array *texCoords, UT_array *tangents);

uint32_t vertex_stream_get_vertex_buffer_binding_count(vertex_stream *stream);

VkVertexInputBindingDescription
vertex_stream_get_vertex_buffer_binding_description(vertex_stream *stream);

VkVertexInputAttributeDescription *
vertex_stream_get_vertex_attribute_descriptions(vertex_stream *stream, uint32_t *count);
