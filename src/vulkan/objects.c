#include "objects.h"
#include "scene/cache.h"

vulkan_buffer *vulkan_buffer_create(vulkan_device *vkd, vulkan_buffer_type type, const void *data,
                                    size_t size) {
  vulkan_buffer *buffer = core_alloc(sizeof(vulkan_buffer));

  buffer->data = data;
  buffer->size = size;

  buffer->vkd = vkd;
  buffer->type = type;
  buffer->resident = false;
  buffer->dirty = true;
  return buffer;
}

void vulkan_buffer_destroy(vulkan_buffer *buffer) {
  if (buffer->resident) {
    vkDestroyBuffer(buffer->vkd->device, buffer->buffer, vka);
    vkFreeMemory(buffer->vkd->device, buffer->bufferMemory, vka);
    buffer->resident = false;
  }
  core_free(buffer);
}

void vulkan_buffer_send_to_device(vulkan_buffer *buffer) {
  if (!buffer->dirty) {
    return;
  }

  VkBufferUsageFlags bufferUsageFlags = 0;
  VkMemoryPropertyFlags memoryPropertyFlags = 0;
  const char *name = "invalid buffer";
  if (buffer->type == vulkan_buffer_type_geometry) {
    bufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    name = "geometry buffer";
  } else if (buffer->type == vulkan_buffer_type_uniform) {
    bufferUsageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    memoryPropertyFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    name = "uniform buffer";
  } else {
    assert(0);
  }

  if (!buffer->resident) {
    vulkan_create_buffer(buffer->vkd, buffer->size, bufferUsageFlags,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &buffer->buffer,
                         &buffer->bufferMemory, name);
    buffer->resident = true;
  }

  bool useStagingBuffer = (memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0;
  if (useStagingBuffer) {
    // TODO: Reuse staging buffer.
    // TODO: Free geometry buffer data if geometry buffer is device local.
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    vulkan_create_buffer(buffer->vkd, buffer->size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         &stagingBuffer, &stagingBufferMemory, "staging buffer for geometry");

    void *data;
    vkMapMemory(buffer->vkd->device, stagingBufferMemory, 0, buffer->size, 0, &data);
    core_memcpy(data, buffer->data, buffer->size);
    vkUnmapMemory(buffer->vkd->device, stagingBufferMemory);

    vulkan_copy_buffer_to_buffer(buffer->vkd, stagingBuffer, buffer->buffer, buffer->size);

    vkDestroyBuffer(buffer->vkd->device, stagingBuffer, vka);
    vkFreeMemory(buffer->vkd->device, stagingBufferMemory, vka);
  } else {
    void *data;
    vkMapMemory(buffer->vkd->device, buffer->bufferMemory, 0, buffer->size, 0, &data);
    core_memcpy(data, buffer->data, buffer->size);
    vkUnmapMemory(buffer->vkd->device, buffer->bufferMemory);
  }
  buffer->dirty = false;
}

uint32_t vulkan_attribute_type_to_stride(vulkan_attribute_type vertexAttributes) {
  if ((vertexAttributes & TexCoordAttribute) != 0) {
    return offsetof(vulkan_interleaved_vertex_stream_element, texCoord) +
           member_size(vulkan_interleaved_vertex_stream_element, texCoord);
  }
  if ((vertexAttributes & ColorAttribute) != 0) {
    return offsetof(vulkan_interleaved_vertex_stream_element, color) +
           member_size(vulkan_interleaved_vertex_stream_element, color);
  }
  if ((vertexAttributes & NormalAttribute) != 0) {
    return offsetof(vulkan_interleaved_vertex_stream_element, normal) +
           member_size(vulkan_interleaved_vertex_stream_element, normal);
  }
  if ((vertexAttributes & PositionAttribute) != 0) {
    return offsetof(vulkan_interleaved_vertex_stream_element, position) +
           member_size(vulkan_interleaved_vertex_stream_element, position);
  }
  panic("unsupported vertex attribute %d", vertexAttributes);
  return 0; // TODO: Unreachable.
}

uint32_t index_type_to_index_stride(vulkan_index_type indexType) {
  if (indexType == vulkan_index_type_uint32) {
    return 4;
  }
  panic("unsupported index type %d", indexType);
  return 0; // TODO unreachable
}

vulkan_index_type index_stride_to_index_type(uint32_t indexStride) {
  switch (indexStride) {
  case 4:
    return vulkan_index_type_uint32;
  default:
    return vulkan_index_type_unknown;
  }
}

VkIndexType stride_to_index_format(uint32_t indexStride) {
  VkIndexType indexType = VK_INDEX_TYPE_NONE_KHR;
  if (indexStride == 2) {
    indexType = VK_INDEX_TYPE_UINT16;
  } else if (indexStride == 4) {
    indexType = VK_INDEX_TYPE_UINT32;
  }
  return indexType;
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
  utarray_alloc(stream->data, vulkan_attribute_type_to_stride(stream->attributes));
  return stream;
}

void vulkan_interleaved_vertex_stream_destroy(vulkan_interleaved_vertex_stream *stream) {
  utarray_free(stream->data);
  core_free(stream);
}

void vulkan_interleaved_vertex_stream_add_primitive(vulkan_interleaved_vertex_stream *stream,
                                                    vulkan_scene_cache *cache) {
  // PERF: Compress stream (overlapping vertex attributes).

  // HIRO: ADD INDICES TO SEPERATE BUFFER, add index offset to cache and batch draw command

  vulkan_scene_cache_set_vertex_stream_offset(cache, utarray_size(stream->data));

  vulkan_data_primitive *primitive = cache->primitive;
  for (size_t idx = 0; idx < primitive->vertexCount; idx++) {
    vulkan_interleaved_vertex_stream_element element =
        vulkan_interleaved_vertex_stream_element_default();
    if ((stream->attributes & PositionAttribute) != 0) {
      vec3 *position = utarray_eltptr(primitive->positions->data, idx);
      glm_vec3_copy(*position, element.position);
    }
    if ((stream->attributes & NormalAttribute) != 0) {
      vec3 *normal = utarray_eltptr(primitive->normals->data, idx);
      glm_vec3_copy(*normal, element.normal);
    }
    if ((stream->attributes & ColorAttribute) != 0) {
      vec3 *color = utarray_eltptr(primitive->colors->data, idx);
      glm_vec3_copy(*color, element.color);
    }
    if ((stream->attributes & TexCoordAttribute) != 0) {
      vec3 *texCoord = utarray_eltptr(primitive->texCoords->data, idx);
      glm_vec2_copy(*texCoord, element.texCoord);
    }
    utarray_push_back(stream->data, &element);
  }
}

void vulkan_interleaved_vertex_stream_add_stream(vulkan_interleaved_vertex_stream *stream,
                                                 vulkan_interleaved_vertex_stream *other) {
  assert(stream->attributes == other->attributes);
  utarray_concat(stream->data, other->data);
}

vulkan_unified_geometry_buffer *vulkan_unified_geometry_buffer_create(vulkan_device *vkd) {
  vulkan_unified_geometry_buffer *geometryBuffer =
      core_alloc(sizeof(vulkan_unified_geometry_buffer));

  geometryBuffer->interleavedVertexStream = NULL;

  geometryBuffer->buffer = vulkan_buffer_create(vkd, vulkan_buffer_type_geometry, NULL, 0);

  geometryBuffer->dirty = true;
  return geometryBuffer;
}

void vulkan_unified_geometry_buffer_destroy(vulkan_unified_geometry_buffer *geometryBuffer) {
  if (geometryBuffer->interleavedVertexStream != NULL) {
    vulkan_interleaved_vertex_stream_destroy(geometryBuffer->interleavedVertexStream);
  }
  vulkan_buffer_destroy(geometryBuffer->buffer);
  core_free(geometryBuffer);
}

void vulkan_unified_geometry_buffer_set_interleaved_vertex_stream(
    vulkan_unified_geometry_buffer *geometryBuffer, vulkan_interleaved_vertex_stream *stream) {
  assert(geometryBuffer->interleavedVertexStream == NULL);
  geometryBuffer->interleavedVertexStream = stream;
}

void vulkan_unified_geometry_buffer_send_to_device(vulkan_unified_geometry_buffer *geometryBuffer) {
  if (!geometryBuffer->dirty || geometryBuffer->interleavedVertexStream == NULL) {
    return;
  }

  geometryBuffer->buffer->data = utarray_front(geometryBuffer->interleavedVertexStream->data);
  geometryBuffer->buffer->size = utarray_size(geometryBuffer->interleavedVertexStream->data);
  geometryBuffer->buffer->dirty = true;
  vulkan_buffer_send_to_device(geometryBuffer->buffer);

  geometryBuffer->dirty = false;
}

void vulkan_unified_geometry_buffer_debug_print(vulkan_unified_geometry_buffer *geometryBuffer) {
  log_debug("UNIFIED GEOMETRY BUFFER:\n");
  assert(geometryBuffer->buffer->size > 0);
  log_debug("size=%d\n", geometryBuffer->buffer->size);
}

vulkan_uniform_buffer *vulkan_uniform_buffer_create(vulkan_device *vkd) {
  vulkan_uniform_buffer *uniformBuffer = core_alloc(sizeof(vulkan_uniform_buffer));

  glm_mat4_identity(uniformBuffer->data.viewMat);
  glm_mat4_identity(uniformBuffer->data.projMat);

  uniformBuffer->buffer = vulkan_buffer_create(vkd, vulkan_buffer_type_uniform,
                                               &uniformBuffer->data, sizeof(uniformBuffer->data));

  uniformBuffer->dirty = true;
  return uniformBuffer;
}

void vulkan_uniform_buffer_destroy(vulkan_uniform_buffer *uniformBuffer) {
  vulkan_buffer_destroy(uniformBuffer->buffer);
  core_free(uniformBuffer);
}

void vulkan_uniform_buffer_send_to_device(vulkan_uniform_buffer *uniformBuffer) {
  uniformBuffer->buffer->dirty = uniformBuffer->dirty;
  vulkan_buffer_send_to_device(uniformBuffer->buffer);
  uniformBuffer->dirty = false;
}
