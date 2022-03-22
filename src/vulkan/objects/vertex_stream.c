#include "vertex_stream.h"
#include "../data/primitive.h"

uint32_t vulkan_attribute_type_to_stride(vulkan_attribute_type vertexAttributes) {
  if ((vertexAttributes & vulkan_attribute_type_texcoord) != 0) {
    return offsetof(vulkan_vertex_stream_element, texCoord) +
           member_size(vulkan_vertex_stream_element, texCoord);
  }
  if ((vertexAttributes & vulkan_attribute_type_color) != 0) {
    return offsetof(vulkan_vertex_stream_element, color) +
           member_size(vulkan_vertex_stream_element, color);
  }
  if ((vertexAttributes & vulkan_attribute_type_normal) != 0) {
    return offsetof(vulkan_vertex_stream_element, normal) +
           member_size(vulkan_vertex_stream_element, normal);
  }
  if ((vertexAttributes & vulkan_attribute_type_position) != 0) {
    return offsetof(vulkan_vertex_stream_element, position) +
           member_size(vulkan_vertex_stream_element, position);
  }
  panic("unsupported vertex attribute %d", vertexAttributes);
  return 0; // TODO: Unreachable.
}

vulkan_vertex_stream_element vulkan_vertex_stream_element_default() {
  vulkan_vertex_stream_element element;
  glm_vec3_zero(element.position);
  glm_vec3_zero(element.normal);
  glm_vec3_zero(element.color);
  glm_vec2_zero(element.texCoord);
  return element;
}

vulkan_vertex_stream *vulkan_vertex_stream_create(vulkan_render_cache_list *renderCacheList) {
  vulkan_vertex_stream *stream = core_alloc(sizeof(vulkan_vertex_stream));

  stream->renderCacheList = renderCacheList;
  stream->indexData = NULL;
  stream->vertexData = NULL;
  stream->dirty = true;

  vulkan_vertex_stream_update(stream);

  return stream;
}

void vulkan_vertex_stream_update(vulkan_vertex_stream *stream) {
  if (!stream->renderCacheList->dirty) {
    return;
  }
  log_debug("update");

  vulkan_render_cache_list_update(stream->renderCacheList);
  assert(stream->renderCacheList->attributes > 0);
  assert(stream->renderCacheList->dirty);

  utarray_realloc(stream->indexData, sizeof(uint32_t));
  utarray_realloc(stream->vertexData,
                  vulkan_attribute_type_to_stride(stream->renderCacheList->attributes));

  // add unique primitives to geometry buffer
  vulkan_render_cache *lastRenderCache = NULL;
  utarray_foreach_elem_deref (vulkan_render_cache *, renderCache, stream->renderCacheList->caches) {
    vulkan_vertex_stream_add_primitive(stream, renderCache, lastRenderCache);
    lastRenderCache = renderCache;
  }

  stream->renderCacheList->dirty = false;
  stream->dirty = true;
}

void vulkan_vertex_stream_destroy(vulkan_vertex_stream *stream) {
  utarray_free(stream->indexData);
  utarray_free(stream->vertexData);
  core_free(stream);
}

void vulkan_vertex_stream_add_primitive(vulkan_vertex_stream *stream,
                                        vulkan_render_cache *renderCache,
                                        vulkan_render_cache *lastRenderCache) {
  // PERF: Compress stream (overlapping vertex attributes).

  if (lastRenderCache != NULL && lastRenderCache->primitive == renderCache->primitive) {
    size_t firstIndexOffset = lastRenderCache->firstIndexOffset;
    size_t firstVertexOffset = lastRenderCache->firstVertexOffset;
    vulkan_render_cache_set_vertex_stream_offsets(renderCache, firstIndexOffset, firstVertexOffset);
    return;
  }

  size_t firstIndexOffset = utarray_len(stream->indexData);
  size_t firstVertexOffset = utarray_len(stream->vertexData);
  vulkan_render_cache_set_vertex_stream_offsets(renderCache, firstIndexOffset, firstVertexOffset);

  vulkan_data_primitive *primitive = renderCache->primitive;

  assert(utarray_len(primitive->indices->data) > 0);
  assert(primitive->indices->componentType == vulkan_data_vertex_attribute_component_uint32_t);
  utarray_concat(stream->indexData, primitive->indices->data);

  for (size_t idx = 0; idx < primitive->vertexCount; idx++) {
    vulkan_vertex_stream_element element = vulkan_vertex_stream_element_default();
    if ((stream->renderCacheList->attributes & vulkan_attribute_type_position) != 0) {
      vec3 *position = utarray_eltptr(primitive->positions->data, idx);
      glm_vec3_copy(*position, element.position);
    }
    if ((stream->renderCacheList->attributes & vulkan_attribute_type_normal) != 0) {
      vec3 *normal = utarray_eltptr(primitive->normals->data, idx);
      glm_vec3_copy(*normal, element.normal);
    }
    if ((stream->renderCacheList->attributes & vulkan_attribute_type_color) != 0) {
      vec3 *color = utarray_eltptr(primitive->colors->data, idx);
      glm_vec3_copy(*color, element.color);
    }
    if ((stream->renderCacheList->attributes & vulkan_attribute_type_texcoord) != 0) {
      vec3 *texCoord = utarray_eltptr(primitive->texCoords->data, idx);
      glm_vec2_copy(*texCoord, element.texCoord);
    }
    utarray_push_back(stream->vertexData, &element);
  }

  stream->dirty = true;
}

size_t vulkan_vertex_stream_get_vertex_buffer_binding_count(vulkan_vertex_stream *stream) {
  return 1;
}

VkVertexInputBindingDescription
vulkan_vertex_stream_get_vertex_buffer_binding_description(vulkan_vertex_stream *stream) {
  VkVertexInputBindingDescription bindingDescription = {0};
  bindingDescription.binding = 0;
  bindingDescription.stride = utarray_eltsize(stream->vertexData);
  assert(bindingDescription.stride > 0);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  // TODO binding with input_rate_instance for instance data? But we have instance data descriptor.
  return bindingDescription;
}

VkVertexInputAttributeDescription *
vulkan_vertex_stream_get_vertex_attribute_descriptions(vulkan_vertex_stream *stream,
                                                       size_t *count) {
  assert(stream->renderCacheList->attributes > 0);
  vulkan_attribute_type attributes = stream->renderCacheList->attributes;
  *count = count_bits(attributes);

  VkVertexInputAttributeDescription *attributeDescriptions =
      core_alloc(*count * sizeof(VkVertexInputAttributeDescription));

  size_t idx = 0;
  size_t offset = 0;
  while (attributes > 0) {
    vulkan_attribute_type type = vulkan_attribute_type_unknown;
    size_t componentNum = 3;
    if ((attributes & vulkan_attribute_type_position) != 0) {
      type = vulkan_attribute_type_position;
    } else if ((attributes & vulkan_attribute_type_normal) != 0) {
      type = vulkan_attribute_type_normal;
    } else if ((attributes & vulkan_attribute_type_color) != 0) {
      type = vulkan_attribute_type_color;
    } else if ((attributes & vulkan_attribute_type_texcoord) != 0) {
      type = vulkan_attribute_type_texcoord;
      componentNum = 2;
    } else {
      panic("unknown attribute");
    }

    VkFormat format = VK_FORMAT_R32_SFLOAT + 3 * (componentNum - 1);
    attributeDescriptions[idx].binding = 0;
    attributeDescriptions[idx].location = idx;
    attributeDescriptions[idx].format = format;
    attributeDescriptions[idx].offset = offset;
    offset += componentNum * sizeof(float);

    attributes &= ~type;
    idx++;
  }

  assert(offset > 0);
  return attributeDescriptions;
}
