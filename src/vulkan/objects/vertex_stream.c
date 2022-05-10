#include "vertex_stream.h"
#include "../assets/primitive.h"

uint32_t vulkan_attribute_type_to_stride(vulkan_attribute_type vertexAttributes) {
  if ((vertexAttributes & vulkan_attribute_type_texcoord) != 0) {
    return offsetof(vulkan_vertex, texCoord) + member_size(vulkan_vertex, texCoord);
  }
  if ((vertexAttributes & vulkan_attribute_type_color) != 0) {
    return offsetof(vulkan_vertex, color) + member_size(vulkan_vertex, color);
  }
  if ((vertexAttributes & vulkan_attribute_type_normal) != 0) {
    return offsetof(vulkan_vertex, normal) + member_size(vulkan_vertex, normal);
  }
  if ((vertexAttributes & vulkan_attribute_type_position) != 0) {
    return offsetof(vulkan_vertex, position) + member_size(vulkan_vertex, position);
  }
  panic("unsupported vertex attribute %d", vertexAttributes);
  return 0; // TODO: Unreachable.
}

vulkan_vertex vulkan_vertex_default() {
  vulkan_vertex element;
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
  if (stream->renderCacheList->primitiveRenderCachesSorted) {
    return;
  }
  log_debug("updating vulkan_vertex_stream");

  // TODO: Reallocates whole geometry buffer, even if only one primitive render cache is added.
  vulkan_render_cache_list_sort_primitive_render_caches(stream->renderCacheList);
  assert(stream->renderCacheList->primitiveRenderCachesSorted);

  assert(stream->renderCacheList->attributes > 0);

  utarray_realloc(stream->indexData, sizeof(uint32_t));
  utarray_realloc(stream->vertexData,
                  vulkan_attribute_type_to_stride(stream->renderCacheList->attributes));

  // add unique primitives to geometry buffer
  vulkan_render_cache *lastRenderCache = NULL;
  utarray_foreach_elem_deref (vulkan_render_cache *, renderCache,
                              stream->renderCacheList->primitiveRenderCaches) {
    vulkan_vertex_stream_add_primitive(stream, renderCache, lastRenderCache);
    lastRenderCache = renderCache;
  }

  stream->dirty = true;
}

void vulkan_vertex_stream_destroy(vulkan_vertex_stream *stream) {
  utarray_free(stream->indexData);
  utarray_free(stream->vertexData);
  core_free(stream);
}

vulkan_vertex_stream_element
vulkan_vertex_stream_add_geometry(vulkan_vertex_stream *stream, uint32_t vertexCount,
                                  UT_array *indices, UT_array *positions, UT_array *normals,
                                  UT_array *colors, UT_array *texCoords) {

  size_t firstIndexOffset = utarray_len(stream->indexData);
  size_t firstVertexOffset = utarray_len(stream->vertexData);
  vulkan_vertex_stream_element vertexStreamElement = {.firstIndexOffset = firstIndexOffset,
                                                      .firstVertexOffset = firstVertexOffset};

  assert(utarray_len(indices) > 0);
  assert(utarray_eltsize(indices) == utarray_eltsize(stream->indexData));
  utarray_concat(stream->indexData, indices);

  assert(positions == NULL || utarray_len(positions) == 0 || utarray_len(positions) == vertexCount);
  assert(normals == NULL || utarray_len(normals) == 0 || utarray_len(normals) == vertexCount);
  assert(colors == NULL || utarray_len(colors) == 0 || utarray_len(colors) == vertexCount);
  assert(texCoords == NULL || utarray_len(texCoords) == 0 || utarray_len(texCoords) == vertexCount);

  for (size_t idx = 0; idx < vertexCount; idx++) {
    vulkan_vertex element = vulkan_vertex_default();
    if (positions != NULL && utarray_len(positions) > 0) {
      vertexStreamElement.attributes |= vulkan_attribute_type_position;
      vec3 *position = utarray_eltptr(positions, idx);
      glm_vec3_copy(*position, element.position);
    }
    if (normals != NULL && utarray_len(normals) > 0) {
      vertexStreamElement.attributes |= vulkan_attribute_type_normal;
      vec3 *normal = utarray_eltptr(normals, idx);
      glm_vec3_copy(*normal, element.normal);
    }
    if (colors != NULL && utarray_len(colors) > 0) {
      vertexStreamElement.attributes |= vulkan_attribute_type_color;
      vec3 *color = utarray_eltptr(colors, idx);
      glm_vec3_copy(*color, element.color);
    }
    if (texCoords != NULL && utarray_len(texCoords) > 0) {
      vertexStreamElement.attributes |= vulkan_attribute_type_texcoord;
      vec3 *texCoord = utarray_eltptr(texCoords, idx);
      glm_vec2_copy(*texCoord, element.texCoord);
    }
    utarray_push_back(stream->vertexData, &element);
  }

  stream->dirty = true;

  return vertexStreamElement;
}

void vulkan_vertex_stream_add_primitive(vulkan_vertex_stream *stream,
                                        vulkan_render_cache *renderCache,
                                        vulkan_render_cache *lastRenderCache) {
  // HIRO Refactor rewrite to return vulkan_vertex_stream_element
  // PERF: Compress stream (overlapping vertex attributes).

  if (lastRenderCache != NULL && lastRenderCache->primitive == renderCache->primitive) {
    size_t firstIndexOffset = lastRenderCache->firstIndexOffset;
    size_t firstVertexOffset = lastRenderCache->firstVertexOffset;
    vulkan_render_cache_set_vertex_stream_offsets(renderCache, firstIndexOffset, firstVertexOffset);
    return;
  }
  vulkan_asset_primitive *primitive = renderCache->primitive;
  assert(primitive->indices->componentType == vulkan_asset_vertex_attribute_component_uint32_t);

  vulkan_vertex_stream_element vertexStreamElement = vulkan_vertex_stream_add_geometry(
      stream, primitive->vertexCount, primitive->indices->data, primitive->positions->data,
      primitive->normals->data, primitive->colors->data, primitive->texCoords->data);

  assert(vertexStreamElement.attributes <= stream->renderCacheList->attributes);
  vulkan_render_cache_set_vertex_stream_offsets(renderCache, vertexStreamElement.firstIndexOffset,
                                                vertexStreamElement.firstVertexOffset);
}

uint32_t vulkan_vertex_stream_get_vertex_buffer_binding_count(vulkan_vertex_stream *stream) {
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
                                                       uint32_t *count) {
  assert(stream->renderCacheList->attributes > 0);
  vulkan_attribute_type attributes = stream->renderCacheList->attributes;
  *count = count_bits(attributes);

  VkVertexInputAttributeDescription *attributeDescriptions =
      core_alloc(*count * sizeof(VkVertexInputAttributeDescription));
  VkFormat format;

  size_t idx = 0;
  while (attributes > 0) {
    vulkan_attribute_type type = vulkan_attribute_type_unknown;
    size_t offset = 0;
    size_t componentNum = 3;
    if ((attributes & vulkan_attribute_type_position) != 0) {
      type = vulkan_attribute_type_position;
      goto end;
    }
    offset += componentNum * sizeof(float);
    if ((attributes & vulkan_attribute_type_normal) != 0) {
      type = vulkan_attribute_type_normal;
      goto end;
    }
    offset += componentNum * sizeof(float);
    if ((attributes & vulkan_attribute_type_color) != 0) {
      type = vulkan_attribute_type_color;
      goto end;
    }
    offset += componentNum * sizeof(float);
    if ((attributes & vulkan_attribute_type_texcoord) != 0) {
      type = vulkan_attribute_type_texcoord;
      componentNum = 2;
    } else {
      panic("unknown attribute");
    }

  end:
    format = VK_FORMAT_R32_SFLOAT + 3 * (componentNum - 1);
    attributeDescriptions[idx].binding = 0;
    attributeDescriptions[idx].location = idx;
    attributeDescriptions[idx].format = format;
    attributeDescriptions[idx].offset = offset;

    attributes &= ~type;
    idx++;
  }

  return attributeDescriptions;
}
