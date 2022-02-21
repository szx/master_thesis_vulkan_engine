#include "vertex_stream.h"
#include "../data/primitive.h"

uint32_t vulkan_attribute_type_to_stride(vulkan_attribute_type vertexAttributes) {
  if ((vertexAttributes & vulkan_attribute_type_texcoord) != 0) {
    return offsetof(vulkan_interleaved_vertex_stream_element, texCoord) +
           member_size(vulkan_interleaved_vertex_stream_element, texCoord);
  }
  if ((vertexAttributes & vulkan_attribute_type_color) != 0) {
    return offsetof(vulkan_interleaved_vertex_stream_element, color) +
           member_size(vulkan_interleaved_vertex_stream_element, color);
  }
  if ((vertexAttributes & vulkan_attribute_type_normal) != 0) {
    return offsetof(vulkan_interleaved_vertex_stream_element, normal) +
           member_size(vulkan_interleaved_vertex_stream_element, normal);
  }
  if ((vertexAttributes & vulkan_attribute_type_position) != 0) {
    return offsetof(vulkan_interleaved_vertex_stream_element, position) +
           member_size(vulkan_interleaved_vertex_stream_element, position);
  }
  panic("unsupported vertex attribute %d", vertexAttributes);
  return 0; // TODO: Unreachable.
}

vulkan_interleaved_vertex_stream_element vulkan_interleaved_vertex_stream_element_default() {
  vulkan_interleaved_vertex_stream_element element;
  glm_vec3_zero(element.position);
  glm_vec3_zero(element.normal);
  glm_vec3_zero(element.color);
  glm_vec2_zero(element.texCoord);
  return element;
}

vulkan_interleaved_vertex_stream *
vulkan_interleaved_vertex_stream_create(vulkan_attribute_type attributes) {
  vulkan_interleaved_vertex_stream *stream = core_alloc(sizeof(vulkan_interleaved_vertex_stream));
  stream->attributes = attributes;
  utarray_alloc(stream->indexData, sizeof(uint32_t));
  utarray_alloc(stream->vertexData, vulkan_attribute_type_to_stride(stream->attributes));
  return stream;
}

void vulkan_interleaved_vertex_stream_destroy(vulkan_interleaved_vertex_stream *stream) {
  utarray_free(stream->indexData);
  utarray_free(stream->vertexData);
  core_free(stream);
}

void vulkan_interleaved_vertex_stream_add_primitive(vulkan_interleaved_vertex_stream *stream,
                                                    vulkan_render_cache *cache) {
  // PERF: Compress stream (overlapping vertex attributes).

  size_t firstIndexOffset = utarray_len(stream->indexData);
  size_t firstVertexOffset = utarray_len(stream->vertexData);
  vulkan_render_cache_set_vertex_stream_offsets(cache, firstIndexOffset, firstVertexOffset);

  vulkan_data_primitive *primitive = cache->primitive;
  vulkan_data_primitive_debug_print(primitive, 0);

  assert(utarray_len(primitive->indices->data) > 0);
  assert(primitive->indices->componentType == vulkan_data_vertex_attribute_component_uint32_t);
  utarray_concat(stream->indexData, primitive->indices->data);

  for (size_t idx = 0; idx < primitive->vertexCount; idx++) {
    vulkan_interleaved_vertex_stream_element element =
        vulkan_interleaved_vertex_stream_element_default();
    if ((stream->attributes & vulkan_attribute_type_position) != 0) {
      vec3 *position = utarray_eltptr(primitive->positions->data, idx);
      glm_vec3_copy(*position, element.position);
    }
    if ((stream->attributes & vulkan_attribute_type_normal) != 0) {
      vec3 *normal = utarray_eltptr(primitive->normals->data, idx);
      glm_vec3_copy(*normal, element.normal);
    }
    if ((stream->attributes & vulkan_attribute_type_color) != 0) {
      vec3 *color = utarray_eltptr(primitive->colors->data, idx);
      glm_vec3_copy(*color, element.color);
    }
    if ((stream->attributes & vulkan_attribute_type_texcoord) != 0) {
      vec3 *texCoord = utarray_eltptr(primitive->texCoords->data, idx);
      glm_vec2_copy(*texCoord, element.texCoord);
    }
    utarray_push_back(stream->vertexData, &element);
  }
}

void vulkan_interleaved_vertex_stream_add_stream(vulkan_interleaved_vertex_stream *stream,
                                                 vulkan_interleaved_vertex_stream *other) {
  assert(stream->attributes == other->attributes);
  utarray_concat(stream->indexData, other->indexData);
  utarray_concat(stream->vertexData, other->vertexData);
}
