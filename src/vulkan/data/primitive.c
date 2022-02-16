#include "primitive.h"
#include "scene.h"

void vulkan_data_primitive_init(vulkan_data_primitive *primitive, vulkan_data_scene *sceneData) {
  primitive->sceneData = sceneData;
  primitive->topology = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
  primitive->vertexCount = 0;
  primitive->positions = NULL;
  primitive->normals = NULL;
  primitive->colors = NULL;
  primitive->texCoords = NULL;
  primitive->indices = NULL;
  primitive->material = NULL;
  DEF_VULKAN_ENTITY(primitive, primitive)
}

void vulkan_data_primitive_deinit(vulkan_data_primitive *primitive) {}

data_key vulkan_data_primitive_calculate_key(vulkan_data_primitive *primitive) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &primitive->topology, sizeof(primitive->topology))
  if (primitive->positions) {
    HASH_UPDATE(hashState, &primitive->positions->key, sizeof(primitive->positions->key))
  }
  if (primitive->normals) {
    HASH_UPDATE(hashState, &primitive->normals->key, sizeof(primitive->normals->key))
  }
  if (primitive->colors) {
    HASH_UPDATE(hashState, &primitive->colors->key, sizeof(primitive->colors->key))
  }
  if (primitive->texCoords) {
    HASH_UPDATE(hashState, &primitive->texCoords->key, sizeof(primitive->texCoords->key))
  }
  if (primitive->indices) {
    HASH_UPDATE(hashState, &primitive->indices->key, sizeof(primitive->indices->key))
  }
  if (primitive->material) {
    HASH_UPDATE(hashState, &primitive->material->key, sizeof(primitive->material->key))
  }
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_data_primitive_serialize(vulkan_data_primitive *primitive, data_asset_db *assetDb) {
  primitive->key = vulkan_data_primitive_calculate_key(primitive);

  vulkan_data_material_serialize(primitive->material, assetDb);
  assert(primitive->material != NULL);
  data_asset_db_insert_primitive_material_key(assetDb, primitive->key, primitive->material->key);

  data_asset_db_insert_primitive_topology_int(assetDb, primitive->key,
                                              data_int_temp(primitive->topology));

  assert(primitive->positions != NULL);
  data_asset_db_insert_primitive_positions_key(assetDb, primitive->key, primitive->positions->key);
  vulkan_data_vertex_attribute_serialize(primitive->positions, assetDb);
  assert(primitive->normals != NULL);
  data_asset_db_insert_primitive_normals_key(assetDb, primitive->key, primitive->normals->key);
  vulkan_data_vertex_attribute_serialize(primitive->normals, assetDb);
  assert(primitive->colors != NULL);
  data_asset_db_insert_primitive_colors_key(assetDb, primitive->key, primitive->colors->key);
  vulkan_data_vertex_attribute_serialize(primitive->colors, assetDb);
  assert(primitive->texCoords != NULL);
  data_asset_db_insert_primitive_texCoords_key(assetDb, primitive->key, primitive->texCoords->key);
  vulkan_data_vertex_attribute_serialize(primitive->texCoords, assetDb);
  assert(primitive->indices != NULL);
  data_asset_db_insert_primitive_indices_key(assetDb, primitive->key, primitive->indices->key);
  vulkan_data_vertex_attribute_serialize(primitive->indices, assetDb);
}

void vulkan_data_primitive_deserialize(vulkan_data_primitive *primitive, data_asset_db *assetDb,
                                       data_key key) {
  primitive->key = key;
  primitive->material = vulkan_data_scene_get_material_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_material_key(assetDb, primitive->key));

  primitive->topology = data_asset_db_select_primitive_topology_int(assetDb, primitive->key).value;
  primitive->positions = vulkan_data_scene_get_vertex_attribute_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_positions_key(assetDb, primitive->key));
  primitive->normals = vulkan_data_scene_get_vertex_attribute_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_normals_key(assetDb, primitive->key));
  primitive->colors = vulkan_data_scene_get_vertex_attribute_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_colors_key(assetDb, primitive->key));
  primitive->texCoords = vulkan_data_scene_get_vertex_attribute_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_texCoords_key(assetDb, primitive->key));
  primitive->indices = vulkan_data_scene_get_vertex_attribute_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_indices_key(assetDb, primitive->key));

  primitive->vertexCount = 0;
  uint32_t positionsCount = utarray_len(primitive->positions->data);
  if (positionsCount > 0) {
    verify(primitive->vertexCount == 0 || primitive->vertexCount == positionsCount);
    primitive->vertexCount = positionsCount;
  }
  uint32_t normalsCount = utarray_len(primitive->normals->data);
  if (normalsCount > 0) {
    verify(primitive->vertexCount == 0 || primitive->vertexCount == normalsCount);
    primitive->vertexCount = normalsCount;
  }
  uint32_t colorsCount = utarray_len(primitive->colors->data);
  if (colorsCount > 0) {
    verify(primitive->vertexCount == 0 || primitive->vertexCount == colorsCount);
    primitive->vertexCount = colorsCount;
  }
  uint32_t texCoordsCount = utarray_len(primitive->texCoords->data);
  if (texCoordsCount > 0) {
    verify(primitive->vertexCount == 0 || primitive->vertexCount == texCoordsCount);
    primitive->vertexCount = texCoordsCount;
  }
}

bool vulkan_data_primitive_vulkan_attributes_match(vulkan_data_primitive *primitive,
                                                   vulkan_data_primitive *other) {
  return primitive->indices == other->indices && primitive->positions == other->positions &&
         primitive->normals == other->normals && primitive->colors == other->colors &&
         primitive->texCoords == other->texCoords;
}

bool vulkan_data_primitive_material_match(vulkan_data_primitive *primitive,
                                          vulkan_data_primitive *other) {
  return primitive->material == other->material;
}

void vulkan_data_primitive_debug_print(vulkan_data_primitive *primitive, int indent) {
  log_debug(INDENT_FORMAT_STRING "primitive: %s\n", INDENT_FORMAT_ARGS(0),
            VkPrimitiveTopology_debug_str(primitive->topology));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), primitive->key);
  vulkan_data_vertex_attribute_debug_print(primitive->indices, indent + 2);
  log_debug(INDENT_FORMAT_STRING "vertices: count=%d\n", INDENT_FORMAT_ARGS(2),
            primitive->vertexCount);
  vulkan_data_vertex_attribute_debug_print(primitive->positions, indent + 2);
  vulkan_data_vertex_attribute_debug_print(primitive->normals, indent + 2);
  vulkan_data_vertex_attribute_debug_print(primitive->colors, indent + 2);
  vulkan_data_vertex_attribute_debug_print(primitive->texCoords, indent + 2);
}
