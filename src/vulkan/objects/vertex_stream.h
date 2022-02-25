/* Vulkan API helper objects. */

#pragma once

#include "common.h"
#include "render_cache_list.h"

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
  vulkan_render_cache_list *renderCacheList; ///< Pointer.

  UT_array *indexData;  ///< uint32_t array
  UT_array *vertexData; ///< vulkan_interleaved_vertex_stream_element array

  bool dirty;
} vulkan_interleaved_vertex_stream;

vulkan_interleaved_vertex_stream *
vulkan_interleaved_vertex_stream_create(vulkan_render_cache_list *renderCacheList);

void vulkan_interleaved_vertex_stream_update(vulkan_interleaved_vertex_stream *stream, bool force);

void vulkan_interleaved_vertex_stream_destroy(vulkan_interleaved_vertex_stream *stream);

void vulkan_interleaved_vertex_stream_add_primitive(vulkan_interleaved_vertex_stream *stream,
                                                    vulkan_render_cache *renderCache,
                                                    vulkan_render_cache *lastRenderCache);

size_t vulkan_interleaved_vertex_stream_get_vertex_buffer_binding_count(
    vulkan_interleaved_vertex_stream *stream);

VkVertexInputBindingDescription
vulkan_interleaved_vertex_stream_get_vertex_buffer_binding_description(
    vulkan_interleaved_vertex_stream *stream);

VkVertexInputAttributeDescription *
vulkan_interleaved_vertex_stream_get_vertex_attribute_descriptions(
    vulkan_interleaved_vertex_stream *stream, size_t *count);
