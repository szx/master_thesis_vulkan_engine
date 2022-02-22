/* Vulkan API helper objects. */

#pragma once

#include "common.h"
#include "render_cache.h"

typedef enum vulkan_attribute_type {
  vulkan_attribute_type_unknown = 1 << 0,
  vulkan_attribute_type_position = 1 << 1,
  vulkan_attribute_type_normal = 1 << 2,
  vulkan_attribute_type_color = 1 << 3,
  vulkan_attribute_type_texcoord = 1 << 4,
  vulkan_attribute_type_tangent = 1 << 5,
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
                                                    vulkan_render_cache *cache);
void vulkan_interleaved_vertex_stream_add_stream(vulkan_interleaved_vertex_stream *stream,
                                                 vulkan_interleaved_vertex_stream *other);

size_t vulkan_interleaved_vertex_stream_get_vertex_buffer_binding_count(
    vulkan_interleaved_vertex_stream *stream);

VkVertexInputBindingDescription
vulkan_interleaved_vertex_stream_get_vertex_buffer_binding_description(
    vulkan_interleaved_vertex_stream *stream);
