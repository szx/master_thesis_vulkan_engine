#include "vertex_attribute.h"
#include "scene.h"

void vulkan_data_vertex_attribute_init(vulkan_data_vertex_attribute *vertexAttribute,
                                       vulkan_data_scene *sceneData) {
  vertexAttribute->componentType = vulkan_data_vertex_attribute_component_uint32_t;
  utarray_alloc(vertexAttribute->data, sizeof(uint32_t));
  DEF_VULKAN_ENTITY(vertex_attribute, vertexAttribute)
}

void vulkan_data_vertex_attribute_deinit(vulkan_data_vertex_attribute *vertexAttribute) {
  utarray_free(vertexAttribute->data);
}

data_key vulkan_data_vertex_attribute_calculate_key(vulkan_data_vertex_attribute *vertexAttribute) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &vertexAttribute->componentType, sizeof(vertexAttribute->componentType))
  HASH_UPDATE(hashState, utarray_front(vertexAttribute->data), utarray_size(vertexAttribute->data))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_data_vertex_attribute_serialize(vulkan_data_vertex_attribute *vertexAttribute,
                                            data_asset_db *assetDb) {
  vertexAttribute->key = vulkan_data_vertex_attribute_calculate_key(vertexAttribute);

  if (vertexAttribute->componentType == vulkan_data_vertex_attribute_component_uint32_t) {
    data_asset_db_insert_vertexAttribute_valuesInt_int_array(
        assetDb, vertexAttribute->key, data_int_array_temp(vertexAttribute->data));
  } else if (vertexAttribute->componentType == vulkan_data_vertex_attribute_component_vec2) {
    data_asset_db_insert_vertexAttribute_valuesVec2_vec2_array(
        assetDb, vertexAttribute->key, data_vec2_array_temp(vertexAttribute->data));
  } else if (vertexAttribute->componentType == vulkan_data_vertex_attribute_component_vec3) {
    data_asset_db_insert_vertexAttribute_valuesVec3_vec3_array(
        assetDb, vertexAttribute->key, data_vec3_array_temp(vertexAttribute->data));
  }
}

void vulkan_data_vertex_attribute_deserialize(vulkan_data_vertex_attribute *vertexAttribute,
                                              data_asset_db *assetDb, data_key key) {
  vertexAttribute->key = key;

  data_int_array valuesInt =
      data_asset_db_select_vertexAttribute_valuesInt_int_array(assetDb, vertexAttribute->key);
  data_vec2_array valuesVec2 =
      data_asset_db_select_vertexAttribute_valuesVec2_vec2_array(assetDb, vertexAttribute->key);
  data_vec3_array valuesVec3 =
      data_asset_db_select_vertexAttribute_valuesVec3_vec3_array(assetDb, vertexAttribute->key);
  if (utarray_len(valuesInt.values) > 0) {
    verify(utarray_len(valuesVec2.values) == 0 && utarray_len(valuesVec3.values) == 0);
    utarray_realloc(vertexAttribute->data, sizeof(uint32_t));
    utarray_resize(vertexAttribute->data, utarray_len(valuesInt.values));
    core_memcpy(utarray_front(vertexAttribute->data), utarray_front(valuesInt.values),
                utarray_size(valuesInt.values));
    vertexAttribute->componentType = vulkan_data_vertex_attribute_component_uint32_t;
  } else if (utarray_len(valuesVec2.values) > 0) {
    verify(utarray_len(valuesInt.values) == 0 && utarray_len(valuesVec3.values) == 0);
    utarray_realloc(vertexAttribute->data, sizeof(vec2));
    utarray_resize(vertexAttribute->data, utarray_len(valuesVec2.values));
    core_memcpy(utarray_front(vertexAttribute->data), utarray_front(valuesVec2.values),
                utarray_size(valuesVec2.values));
    vertexAttribute->componentType = vulkan_data_vertex_attribute_component_vec2;
  } else if (utarray_len(valuesVec3.values) > 0) {
    verify(utarray_len(valuesInt.values) == 0 && utarray_len(valuesVec2.values) == 0);
    utarray_realloc(vertexAttribute->data, sizeof(vec3));
    utarray_resize(vertexAttribute->data, utarray_len(valuesVec3.values));
    core_memcpy(utarray_front(vertexAttribute->data), utarray_front(valuesVec3.values),
                utarray_size(valuesVec3.values));
    vertexAttribute->componentType = vulkan_data_vertex_attribute_component_vec3;
  } else {
    verify(0);
  }
  data_int_array_deinit(&valuesInt);
  data_vec2_array_deinit(&valuesVec2);
  data_vec3_array_deinit(&valuesVec3);
}

void vulkan_data_vertex_attribute_debug_print(vulkan_data_vertex_attribute *vertexAttribute,
                                              int indent) {
  log_debug(INDENT_FORMAT_STRING "vertexAttribute:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), vertexAttribute->key);
  size_t vertexIdx = 0;
  if (vertexAttribute->componentType == vulkan_data_vertex_attribute_component_uint32_t) {
    utarray_foreach_elem_deref (uint32_t, value, vertexAttribute->data) {
      log_debug(INDENT_FORMAT_STRING "%d: %d\n", INDENT_FORMAT_ARGS(4), vertexIdx++, value);
    }
  } else if (vertexAttribute->componentType == vulkan_data_vertex_attribute_component_vec2) {
    utarray_foreach_elem_it (vec2 *, value, vertexAttribute->data) {
      log_debug(INDENT_FORMAT_STRING "%d: %f %f\n", INDENT_FORMAT_ARGS(4), vertexIdx++, (*value)[0],
                (*value)[1]);
    }
  } else if (vertexAttribute->componentType == vulkan_data_vertex_attribute_component_vec3) {
    utarray_foreach_elem_it (vec3 *, value, vertexAttribute->data) {
      log_debug(INDENT_FORMAT_STRING "%d: %f %f %f\n", INDENT_FORMAT_ARGS(4), vertexIdx++,
                (*value)[0], (*value)[1], (*value)[2]);
    }
  }
}
