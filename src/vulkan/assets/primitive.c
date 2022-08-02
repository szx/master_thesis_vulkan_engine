#include "primitive.h"
#include "../scene/data.h"

void asset_primitive_init(asset_primitive *primitive, scene_data *sceneData) {
  primitive->sceneData = sceneData;
  primitive->topology = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
  primitive->vertexCount = 0;
  primitive->positions = NULL;
  primitive->normals = NULL;
  primitive->colors = NULL;
  primitive->texCoords = NULL;
  primitive->tangents = NULL;
  primitive->indices = NULL;
  primitive->geometryHash = 0;
  primitive->attributes = vertex_attribute_type_unknown;
  primitive->material = NULL;
  VULKAN_ASSET_FIELD_DEFS(primitive, primitive)
}

void asset_primitive_deinit(asset_primitive *primitive) {}

data_key asset_primitive_calculate_key(asset_primitive *primitive) {
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
  if (primitive->tangents) {
    HASH_UPDATE(hashState, &primitive->tangents->key, sizeof(primitive->tangents->key))
  }
  if (primitive->indices) {
    HASH_UPDATE(hashState, &primitive->indices->key, sizeof(primitive->indices->key))
  }
  HASH_DIGEST(hashState, primitive->geometryHash);
  if (primitive->material) {
    HASH_UPDATE(hashState, &primitive->material->key, sizeof(primitive->material->key))
  }
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void asset_primitive_serialize(asset_primitive *primitive, data_asset_db *assetDb) {
  primitive->key = asset_primitive_calculate_key(primitive);

  asset_material_serialize(primitive->material, assetDb);
  assert(primitive->material != NULL);
  data_asset_db_insert_primitive_material_key(assetDb, primitive->key, primitive->material->key);

  data_asset_db_insert_primitive_topology_int(assetDb, primitive->key,
                                              data_int_temp(primitive->topology));

  assert(primitive->positions != NULL);
  data_asset_db_insert_primitive_positions_key(assetDb, primitive->key, primitive->positions->key);
  asset_vertex_attribute_serialize(primitive->positions, assetDb);
  assert(primitive->normals != NULL);
  data_asset_db_insert_primitive_normals_key(assetDb, primitive->key, primitive->normals->key);
  asset_vertex_attribute_serialize(primitive->normals, assetDb);
  assert(primitive->colors != NULL);
  data_asset_db_insert_primitive_colors_key(assetDb, primitive->key, primitive->colors->key);
  asset_vertex_attribute_serialize(primitive->colors, assetDb);
  assert(primitive->texCoords != NULL);
  data_asset_db_insert_primitive_texCoords_key(assetDb, primitive->key, primitive->texCoords->key);
  asset_vertex_attribute_serialize(primitive->texCoords, assetDb);
  assert(primitive->tangents != NULL);
  data_asset_db_insert_primitive_tangents_key(assetDb, primitive->key, primitive->tangents->key);
  asset_vertex_attribute_serialize(primitive->tangents, assetDb);
  assert(primitive->indices != NULL);
  data_asset_db_insert_primitive_indices_key(assetDb, primitive->key, primitive->indices->key);
  asset_vertex_attribute_serialize(primitive->indices, assetDb);
}

void asset_primitive_deserialize(asset_primitive *primitive, data_asset_db *assetDb, data_key key) {
  primitive->key = key;
  primitive->material = scene_data_get_material_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_material_key(assetDb, primitive->key));

  primitive->topology = data_asset_db_select_primitive_topology_int(assetDb, primitive->key).value;
  primitive->attributes = vertex_attribute_type_unknown;

  primitive->positions = scene_data_get_vertex_attribute_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_positions_key(assetDb, primitive->key));
  primitive->attributes |=
      utarray_len(primitive->positions->data) > 0 ? vertex_attribute_type_position : 0;

  primitive->normals = scene_data_get_vertex_attribute_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_normals_key(assetDb, primitive->key));
  primitive->attributes |=
      utarray_len(primitive->normals->data) > 0 ? vertex_attribute_type_normal : 0;

  primitive->colors = scene_data_get_vertex_attribute_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_colors_key(assetDb, primitive->key));
  primitive->attributes |=
      utarray_len(primitive->colors->data) > 0 ? vertex_attribute_type_color : 0;

  primitive->texCoords = scene_data_get_vertex_attribute_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_texCoords_key(assetDb, primitive->key));
  primitive->attributes |=
      utarray_len(primitive->texCoords->data) > 0 ? vertex_attribute_type_texcoord : 0;

  primitive->tangents = scene_data_get_vertex_attribute_by_key(
      primitive->sceneData, assetDb,
      data_asset_db_select_primitive_tangents_key(assetDb, primitive->key));
  primitive->attributes |=
      utarray_len(primitive->tangents->data) > 0 ? vertex_attribute_type_tangent : 0;

  primitive->indices = scene_data_get_vertex_attribute_by_key(
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
  uint32_t tangentsCount = utarray_len(primitive->tangents->data);
  if (tangentsCount > 0) {
    verify(primitive->vertexCount == 0 || primitive->vertexCount == tangentsCount);
    primitive->vertexCount = tangentsCount;
  }
}

void asset_primitive_debug_print(asset_primitive *primitive, int indent) {
  log_debug(INDENT_FORMAT_STRING "primitive: %s\n", INDENT_FORMAT_ARGS(0),
            VkPrimitiveTopology_debug_str(primitive->topology));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), primitive->key);
  log_debug(INDENT_FORMAT_STRING "geometry_hash=%zu", INDENT_FORMAT_ARGS(2),
            primitive->geometryHash);
  asset_vertex_attribute_debug_print(primitive->indices, indent + 2);
  log_debug(INDENT_FORMAT_STRING "vertices: count=%d\n", INDENT_FORMAT_ARGS(2),
            primitive->vertexCount);
  asset_vertex_attribute_debug_print(primitive->positions, indent + 2);
  asset_vertex_attribute_debug_print(primitive->normals, indent + 2);
  asset_vertex_attribute_debug_print(primitive->colors, indent + 2);
  asset_vertex_attribute_debug_print(primitive->texCoords, indent + 2);
  asset_vertex_attribute_debug_print(primitive->tangents, indent + 2);
}

asset_primitive *asset_primitive_create_from_geometry(scene_data *sceneData,
                                                      VkPrimitiveTopology topology,
                                                      uint32_t vertexCount, uint32_t *indices,
                                                      vec3 *positions, vec3 *normals, vec3 *colors,
                                                      vec2 *texCoords, vec4 *tangents) {
  asset_primitive *primitive = core_alloc(sizeof(asset_primitive));
  asset_primitive_init(primitive, sceneData);

  primitive->topology = topology;
  primitive->vertexCount = vertexCount;
  primitive->attributes = 0;
#define VERTEX_ATTRIBUTE(_name, _type, _attributeType)                                             \
  {                                                                                                \
    asset_vertex_attribute *attribute = core_alloc(sizeof(asset_vertex_attribute));                \
    asset_vertex_attribute_init(attribute, sceneData);                                             \
    if (_name != NULL) {                                                                           \
      attribute->componentType = asset_vertex_attribute_component_##_type;                         \
      utarray_realloc(attribute->data, sizeof(_type));                                             \
      utarray_resize(attribute->data, primitive->vertexCount);                                     \
      core_memcpy(utarray_front(attribute->data), _name, utarray_size(attribute->data));           \
      primitive->attributes |= _attributeType;                                                     \
    }                                                                                              \
    primitive->_name = scene_data_add_vertex_attribute(sceneData, attribute);                      \
  }
  VERTEX_ATTRIBUTE(indices, uint32_t, vertex_attribute_type_unknown)
  VERTEX_ATTRIBUTE(positions, vec3, vertex_attribute_type_position)
  VERTEX_ATTRIBUTE(normals, vec3, vertex_attribute_type_normal)
  VERTEX_ATTRIBUTE(colors, vec3, vertex_attribute_type_color)
  VERTEX_ATTRIBUTE(texCoords, vec2, vertex_attribute_type_texcoord)
  VERTEX_ATTRIBUTE(tangents, vec4, vertex_attribute_type_tangent)
#undef VERTEX_ATTRIBUTE

  primitive->material = scene_data_get_default_material(sceneData);

  return scene_data_add_primitive(sceneData, primitive);
}

bool asset_primitive_vertex_attributes_match(asset_primitive *primitive, asset_primitive *other) {
  return primitive->topology == other->topology && primitive->indices == other->indices &&
         primitive->positions == other->positions && primitive->normals == other->normals &&
         primitive->colors == other->colors && primitive->texCoords == other->texCoords &&
         primitive->tangents == other->tangents;
}

aabb asset_primitive_calculate_aabb(asset_primitive *primitive) {
  aabb aabb;
  glm_vec4_fill(aabb.min, FLT_MAX); // FLT_MIN is minimum number > 0
  glm_vec4_fill(aabb.max, -FLT_MAX);
  utarray_foreach_elem_it (vec3 *, position, primitive->positions->data) {
    aabb_add_vec3(&aabb, *position);
  }
  return aabb;
}
