#include "primitive.h"

void vulkan_primitive_data_init(vulkan_primitive_data *primitive) {
  primitive->topology = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
  primitive->vertexCount = 0;
  utarray_alloc(primitive->positions, sizeof(vec3));
  utarray_alloc(primitive->normals, sizeof(vec3));
  utarray_alloc(primitive->colors, sizeof(vec3));
  utarray_alloc(primitive->texCoords, sizeof(vec2));
  utarray_alloc(primitive->indices, sizeof(uint32_t));
  data_key_init(&primitive->hash, 0);
}

void vulkan_primitive_data_deinit(vulkan_primitive_data *primitive) {
  utarray_free(primitive->positions);
  utarray_free(primitive->normals);
  utarray_free(primitive->colors);
  utarray_free(primitive->texCoords);
  utarray_free(primitive->indices);
}

data_key vulkan_primitive_data_calculate_key(vulkan_primitive_data *primitive) {
  hash_t value;
  // TODO: Currently primitive hash is produced using all topology + indices + vertex attributes.
  //       This eliminates duplicated primitives with EXACTLY the same structure, but still
  //       results in e.g. nodes with same vertex attributes but different indices are stored as
  //       separate entries in asset database, even if they share vertex attributes.
  //       It could be optimized with separate hashes and tables for each vertex attribute and
  //       foreign keys in primitive table. Not sure if its worth additional work.
  HASH_START(hashState)
  HASH_UPDATE(hashState, &primitive->topology, sizeof(primitive->topology))
#define HASH_UPDATE_FUNC(_i, _array)                                                               \
  HASH_UPDATE(hashState, utarray_front(_array), utarray_size(_array))
  MACRO_FOREACH(HASH_UPDATE_FUNC, primitive->indices, primitive->positions, primitive->normals,
                primitive->colors, primitive->texCoords)
#undef HASH_UPDATE_FUNC
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_primitive_data_serialize(vulkan_primitive_data *primitive, data_asset_db *assetDb) {
  primitive->hash = vulkan_primitive_data_calculate_key(primitive);
  data_asset_db_insert_primitive_topology_int(assetDb, primitive->hash,
                                              data_int_temp(primitive->topology));
  data_asset_db_insert_primitive_indices_int_array(assetDb, primitive->hash,
                                                   data_int_array_temp(primitive->indices));
  data_asset_db_insert_primitive_positions_vec3_array(assetDb, primitive->hash,
                                                      data_vec3_array_temp(primitive->positions));
  data_asset_db_insert_primitive_normals_vec3_array(assetDb, primitive->hash,
                                                    data_vec3_array_temp(primitive->normals));
  data_asset_db_insert_primitive_colors_vec3_array(assetDb, primitive->hash,
                                                   data_vec3_array_temp(primitive->colors));
  data_asset_db_insert_primitive_texCoords_vec2_array(assetDb, primitive->hash,
                                                      data_vec2_array_temp(primitive->texCoords));
}

void vulkan_primitive_data_deserialize(vulkan_primitive_data *primitive, data_asset_db *assetDb,
                                       data_key key) {
  primitive->hash = key;
  primitive->topology = data_asset_db_select_primitive_topology_int(assetDb, primitive->hash).value;

#define SELECT_ARRAY(_name, _type)                                                                 \
  data_##_type _name##Blob =                                                                       \
      data_asset_db_select_primitive_##_name##_##_type(assetDb, primitive->hash);                  \
  utarray_resize(primitive->_name, utarray_len(_name##Blob.values));                               \
  core_memcpy(primitive->_name->d, _name##Blob.values->d, utarray_size(primitive->_name));         \
  data_##_type##_deinit(&_name##Blob);

  SELECT_ARRAY(indices, int_array);

#define SELECT_VERTEX_ATTRIBUTE(_name, _type)                                                      \
  SELECT_ARRAY(_name, _type)                                                                       \
  if (primitive->vertexCount == 0) {                                                               \
    log_debug("primitive->vertexCount=%zu", utarray_len(primitive->_name));                        \
    primitive->vertexCount = utarray_len(primitive->_name);                                        \
  } else if (utarray_len(primitive->_name) > 0) {                                                  \
    verify(primitive->vertexCount == utarray_len(primitive->_name));                               \
  }

  SELECT_VERTEX_ATTRIBUTE(positions, vec3_array)
  SELECT_VERTEX_ATTRIBUTE(normals, vec3_array)
  SELECT_VERTEX_ATTRIBUTE(colors, vec3_array)
  SELECT_VERTEX_ATTRIBUTE(texCoords, vec2_array)

#undef COPY_VERTEX_ATTRIBUTE
#undef COPY_BLOB_TO_ARRAY
}

void vulkan_primitive_data_debug_print(vulkan_primitive_data *primitive) {
  log_debug("primitive: %s\n", VkPrimitiveTopology_debug_str(primitive->topology));
  log_debug("  hash=%zu", primitive->hash);
  log_debug("  index: count=%d\n", utarray_len(primitive->indices));
  void *index = NULL;
  while ((index = utarray_next(primitive->indices, index))) {
    log_debug("    %u\n", *(uint32_t *)index);
  }
  log_debug("  vertices: count=%d\n", primitive->vertexCount, primitive->vertexCount);
  uint32_t vertexIdx = 0;
  vec3 *position = NULL;
  while ((position = utarray_next(primitive->positions, position))) {
    log_debug("      position %d: %f %f %f\n", vertexIdx++, (*position)[0], (*position)[1],
              (*position)[2]);
  }
  vertexIdx = 0;
  vec3 *normal = NULL;
  while ((normal = utarray_next(primitive->normals, normal))) {
    log_debug("      normal %d: %f %f %f\n", vertexIdx++, (*normal)[0], (*normal)[1], (*normal)[2]);
  }
  vertexIdx = 0;
  vec3 *color = NULL;
  while ((color = utarray_next(primitive->colors, color))) {
    log_debug("      color %d: %f %f %f\n", vertexIdx++, (*color)[0], (*color)[1], (*color)[2]);
  }
  vertexIdx = 0;
  vec2 *texCoord = NULL;
  while ((texCoord = utarray_next(primitive->texCoords, texCoord))) {
    log_debug("      texCoord %d: %f %f\n", vertexIdx++, (*texCoord)[0], (*texCoord)[1]);
  }
}