#include "vertex_stream.h"

uint32_t vertex_attribute_type_to_stride(vertex_attribute_type vertexAttributes) {
  if ((vertexAttributes & vertex_attribute_type_tangent) != 0) {
    return offsetof(vertex, tangent) + member_size(vertex, tangent);
  }
  if ((vertexAttributes & vertex_attribute_type_texcoord) != 0) {
    return offsetof(vertex, texCoord) + member_size(vertex, texCoord);
  }
  if ((vertexAttributes & vertex_attribute_type_color) != 0) {
    return offsetof(vertex, color) + member_size(vertex, color);
  }
  if ((vertexAttributes & vertex_attribute_type_normal) != 0) {
    return offsetof(vertex, normal) + member_size(vertex, normal);
  }
  if ((vertexAttributes & vertex_attribute_type_position) != 0) {
    return offsetof(vertex, position) + member_size(vertex, position);
  }
  UNREACHABLE;
}

vertex vertex_default() {
  vertex element;
  glm_vec3_zero(element.position);
  glm_vec3_zero(element.normal);
  glm_vec3_zero(element.color);
  glm_vec2_zero(element.texCoord);
  glm_vec4_zero(element.tangent);
  return element;
}

vertex_stream *vertex_stream_create(vertex_attribute_type attributes) {
  vertex_stream *stream = core_alloc(sizeof(vertex_stream));

  assert(attributes > 0);
  stream->attributes = attributes;
  utarray_alloc(stream->indexData, sizeof(uint32_t));
  utarray_alloc(stream->vertexData, vertex_attribute_type_to_stride(stream->attributes));
  stream->dirty = true;

  vertex_stream_update(stream);

  return stream;
}

void vertex_stream_update(vertex_stream *stream) {
  // No-op.
  // Unified geometry buffer is responsible for synchronizing vertex stream.
}

void vertex_stream_destroy(vertex_stream *stream) {
  utarray_free(stream->indexData);
  utarray_free(stream->vertexData);
  core_free(stream);
}

vertex_stream_element vertex_stream_add_geometry(vertex_stream *stream, uint32_t vertexCount,
                                                 UT_array *indices, UT_array *positions,
                                                 UT_array *normals, UT_array *colors,
                                                 UT_array *texCoords, UT_array *tangents) {

  size_t firstIndexOffset = utarray_len(stream->indexData);
  size_t firstVertexOffset = utarray_len(stream->vertexData);
  vertex_stream_element vertexStreamElement = {.firstIndexOffset = firstIndexOffset,
                                               .firstVertexOffset = firstVertexOffset};

  assert(utarray_len(indices) > 0);
  assert(utarray_eltsize(indices) == utarray_eltsize(stream->indexData));
  utarray_concat(stream->indexData, indices);

  assert(positions == NULL || utarray_len(positions) == 0 || utarray_len(positions) == vertexCount);
  assert(normals == NULL || utarray_len(normals) == 0 || utarray_len(normals) == vertexCount);
  assert(colors == NULL || utarray_len(colors) == 0 || utarray_len(colors) == vertexCount);
  assert(texCoords == NULL || utarray_len(texCoords) == 0 || utarray_len(texCoords) == vertexCount);
  assert(tangents == NULL || utarray_len(tangents) == 0 || utarray_len(tangents) == vertexCount);

  for (size_t idx = 0; idx < vertexCount; idx++) {
    vertex element = vertex_default();
    if (positions != NULL && utarray_len(positions) > 0) {
      vertexStreamElement.attributes |= vertex_attribute_type_position;
      vec3 *position = utarray_eltptr(positions, idx);
      glm_vec3_copy(*position, element.position);
    }
    if (normals != NULL && utarray_len(normals) > 0) {
      vertexStreamElement.attributes |= vertex_attribute_type_normal;
      vec3 *normal = utarray_eltptr(normals, idx);
      glm_vec3_copy(*normal, element.normal);
    }
    if (colors != NULL && utarray_len(colors) > 0) {
      vertexStreamElement.attributes |= vertex_attribute_type_color;
      vec3 *color = utarray_eltptr(colors, idx);
      glm_vec3_copy(*color, element.color);
    }
    if (texCoords != NULL && utarray_len(texCoords) > 0) {
      vertexStreamElement.attributes |= vertex_attribute_type_texcoord;
      vec3 *texCoord = utarray_eltptr(texCoords, idx);
      glm_vec2_copy(*texCoord, element.texCoord);
    }
    if (tangents != NULL && utarray_len(tangents) > 0) {
      vertexStreamElement.attributes |= vertex_attribute_type_tangent;
      vec4 *tangent = utarray_eltptr(tangents, idx);
      glm_vec4_copy(*tangent, element.tangent);
    }
    assert(vertexStreamElement.attributes <= stream->attributes);
    utarray_push_back(stream->vertexData, &element);
  }

  stream->dirty = true;

  return vertexStreamElement;
}

uint32_t vertex_stream_get_vertex_buffer_binding_count(vertex_stream *stream) { return 1; }

VkVertexInputBindingDescription
vertex_stream_get_vertex_buffer_binding_description(vertex_stream *stream) {
  VkVertexInputBindingDescription bindingDescription = {0};
  bindingDescription.binding = 0;
  bindingDescription.stride = utarray_eltsize(stream->vertexData);
  assert(bindingDescription.stride > 0);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  // TODO binding with input_rate_instance for instance data? But we have instance data descriptor.
  return bindingDescription;
}

VkVertexInputAttributeDescription *
vertex_stream_get_vertex_attribute_descriptions(vertex_stream *stream, uint32_t *count) {
  assert(stream->attributes > 0);
  vertex_attribute_type attributes = stream->attributes;
  *count = count_bits(attributes);

  VkVertexInputAttributeDescription *attributeDescriptions =
      core_alloc(*count * sizeof(VkVertexInputAttributeDescription));
  VkFormat format;

  size_t idx = 0;
  while (attributes > 0) {
    vertex_attribute_type type;

    size_t offset = 0;
    size_t componentNum = 3;
    if ((attributes & vertex_attribute_type_position) != 0) {
      type = vertex_attribute_type_position;
      goto end;
    }

    offset += componentNum * sizeof(float);
    componentNum = 3;
    if ((attributes & vertex_attribute_type_normal) != 0) {
      type = vertex_attribute_type_normal;
      goto end;
    }

    offset += componentNum * sizeof(float);
    componentNum = 3;
    if ((attributes & vertex_attribute_type_color) != 0) {
      type = vertex_attribute_type_color;
      goto end;
    }

    offset += componentNum * sizeof(float);
    componentNum = 2;
    if ((attributes & vertex_attribute_type_texcoord) != 0) {
      type = vertex_attribute_type_texcoord;
      goto end;
    }

    offset += componentNum * sizeof(float);
    componentNum = 4;
    if ((attributes & vertex_attribute_type_tangent) != 0) {
      type = vertex_attribute_type_tangent;
      goto end;
    }

    UNREACHABLE;

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
