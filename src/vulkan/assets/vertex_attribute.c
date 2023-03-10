#include "vertex_attribute.h"
#include "../scene/data.h"

void asset_vertex_attribute_init(asset_vertex_attribute *vertexAttribute,
                                        scene_data *sceneData) {
  vertexAttribute->componentType = asset_vertex_attribute_component_uint32_t;
  utarray_alloc(vertexAttribute->data, sizeof(uint32_t));
  VULKAN_ASSET_FIELD_DEFS(vertex_attribute, vertexAttribute)
}

void asset_vertex_attribute_deinit(asset_vertex_attribute *vertexAttribute) {
  utarray_free(vertexAttribute->data);
}

data_key
asset_vertex_attribute_calculate_key(asset_vertex_attribute *vertexAttribute) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &vertexAttribute->componentType, sizeof(vertexAttribute->componentType))
  HASH_UPDATE(hashState, utarray_front(vertexAttribute->data), utarray_size(vertexAttribute->data))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void asset_vertex_attribute_serialize(asset_vertex_attribute *vertexAttribute, asset_db *assetDb) {
  vertexAttribute->key = asset_vertex_attribute_calculate_key(vertexAttribute);

  if (vertexAttribute->componentType == asset_vertex_attribute_component_uint32_t) {
    asset_db_insert_vertexAttribute_valuesInt_int_array(assetDb, vertexAttribute->key,
                                                        data_int_array_temp(vertexAttribute->data));
  } else if (vertexAttribute->componentType == asset_vertex_attribute_component_vec2) {
    asset_db_insert_vertexAttribute_valuesVec2_vec2_array(
        assetDb, vertexAttribute->key, data_vec2_array_temp(vertexAttribute->data));
  } else if (vertexAttribute->componentType == asset_vertex_attribute_component_vec3) {
    asset_db_insert_vertexAttribute_valuesVec3_vec3_array(
        assetDb, vertexAttribute->key, data_vec3_array_temp(vertexAttribute->data));
  } else if (vertexAttribute->componentType == asset_vertex_attribute_component_vec4) {
    asset_db_insert_vertexAttribute_valuesVec4_vec4_array(
        assetDb, vertexAttribute->key, data_vec4_array_temp(vertexAttribute->data));
  }
}

void asset_vertex_attribute_deserialize(asset_vertex_attribute *vertexAttribute, asset_db *assetDb,
                                        data_key key) {
  vertexAttribute->key = key;

  data_int_array valuesInt =
      asset_db_select_vertexAttribute_valuesInt_int_array(assetDb, vertexAttribute->key);
  data_vec2_array valuesVec2 =
      asset_db_select_vertexAttribute_valuesVec2_vec2_array(assetDb, vertexAttribute->key);
  data_vec3_array valuesVec3 =
      asset_db_select_vertexAttribute_valuesVec3_vec3_array(assetDb, vertexAttribute->key);
  data_vec4_array valuesVec4 =
      asset_db_select_vertexAttribute_valuesVec4_vec4_array(assetDb, vertexAttribute->key);
  if (utarray_len(valuesInt.values) > 0) {
    verify(utarray_len(valuesVec2.values) == 0 && utarray_len(valuesVec3.values) == 0);
    utarray_realloc(vertexAttribute->data, sizeof(uint32_t));
    utarray_resize(vertexAttribute->data, utarray_len(valuesInt.values));
    core_memcpy(utarray_front(vertexAttribute->data), utarray_front(valuesInt.values),
                utarray_size(valuesInt.values));
    vertexAttribute->componentType = asset_vertex_attribute_component_uint32_t;
  } else if (utarray_len(valuesVec2.values) > 0) {
    verify(utarray_len(valuesInt.values) == 0 && utarray_len(valuesVec3.values) == 0);
    utarray_realloc(vertexAttribute->data, sizeof(vec2));
    utarray_resize(vertexAttribute->data, utarray_len(valuesVec2.values));
    core_memcpy(utarray_front(vertexAttribute->data), utarray_front(valuesVec2.values),
                utarray_size(valuesVec2.values));
    vertexAttribute->componentType = asset_vertex_attribute_component_vec2;
  } else if (utarray_len(valuesVec3.values) > 0) {
    verify(utarray_len(valuesInt.values) == 0 && utarray_len(valuesVec2.values) == 0);
    utarray_realloc(vertexAttribute->data, sizeof(vec3));
    utarray_resize(vertexAttribute->data, utarray_len(valuesVec3.values));
    core_memcpy(utarray_front(vertexAttribute->data), utarray_front(valuesVec3.values),
                utarray_size(valuesVec3.values));
    vertexAttribute->componentType = asset_vertex_attribute_component_vec3;
  } else if (utarray_len(valuesVec4.values) > 0) {
    verify(utarray_len(valuesInt.values) == 0 && utarray_len(valuesVec2.values) == 0 &&
           utarray_len(valuesVec3.values) == 0);
    utarray_realloc(vertexAttribute->data, sizeof(vec4));
    utarray_resize(vertexAttribute->data, utarray_len(valuesVec4.values));
    core_memcpy(utarray_front(vertexAttribute->data), utarray_front(valuesVec4.values),
                utarray_size(valuesVec4.values));
    vertexAttribute->componentType = asset_vertex_attribute_component_vec4;
  } else {
    verify(0);
  }
  data_int_array_deinit(&valuesInt);
  data_vec2_array_deinit(&valuesVec2);
  data_vec3_array_deinit(&valuesVec3);
  data_vec4_array_deinit(&valuesVec4);
}

void asset_vertex_attribute_debug_print(asset_vertex_attribute *vertexAttribute,
                                               int indent) {
  log_debug(INDENT_FORMAT_STRING "vertexAttribute:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), vertexAttribute->key);
  size_t vertexIdx = 0;
  if (vertexAttribute->componentType == asset_vertex_attribute_component_uint32_t) {
    utarray_foreach_elem_deref (uint32_t, value, vertexAttribute->data) {
      log_debug(INDENT_FORMAT_STRING "%d: %d\n", INDENT_FORMAT_ARGS(4), vertexIdx++, value);
    }
  } else if (vertexAttribute->componentType == asset_vertex_attribute_component_vec2) {
    utarray_foreach_elem_it (vec2 *, value, vertexAttribute->data) {
      log_debug(INDENT_FORMAT_STRING "%d: %f %f\n", INDENT_FORMAT_ARGS(4), vertexIdx++, (*value)[0],
                (*value)[1]);
    }
  } else if (vertexAttribute->componentType == asset_vertex_attribute_component_vec3) {
    utarray_foreach_elem_it (vec3 *, value, vertexAttribute->data) {
      log_debug(INDENT_FORMAT_STRING "%d: %f %f %f\n", INDENT_FORMAT_ARGS(4), vertexIdx++,
                (*value)[0], (*value)[1], (*value)[2]);
    }
  } else if (vertexAttribute->componentType == asset_vertex_attribute_component_vec4) {
    utarray_foreach_elem_it (vec4 *, value, vertexAttribute->data) {
      log_debug(INDENT_FORMAT_STRING "%d: %f %f %f %f\n", INDENT_FORMAT_ARGS(4), vertexIdx++,
                (*value)[0], (*value)[1], (*value)[2], (*value)[3]);
    }
  }
}
