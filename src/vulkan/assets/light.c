#include "light.h"
#include "../scene/data.h"

void asset_direct_light_init(asset_direct_light *directLight, scene_data *sceneData) {
  directLight->type = direct_light_type_point;
  glm_vec3_copy(GLM_VEC3_ZERO, directLight->position);
  directLight->innerConeAngle = 0;
  directLight->outerConeAngle = 0;
  directLight->intensity = 0;
  directLight->range = 0;
  glm_vec3_copy(GLM_VEC3_ZERO, directLight->color);

  VULKAN_ASSET_FIELD_DEFS(direct_light, directLight)
}

void asset_direct_light_deinit(asset_direct_light *directLight) {}

data_key asset_direct_light_calculate_key(asset_direct_light *directLight) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &directLight->type, sizeof(directLight->type))
  HASH_UPDATE(hashState, &directLight->position, sizeof(directLight->position))
  HASH_UPDATE(hashState, &directLight->innerConeAngle, sizeof(directLight->innerConeAngle))
  HASH_UPDATE(hashState, &directLight->outerConeAngle, sizeof(directLight->outerConeAngle))
  HASH_UPDATE(hashState, &directLight->intensity, sizeof(directLight->intensity))
  HASH_UPDATE(hashState, &directLight->range, sizeof(directLight->range))
  HASH_UPDATE(hashState, &directLight->color, sizeof(directLight->color))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState);
  return (data_key){value};
}

void asset_direct_light_serialize(asset_direct_light *directLight, data_asset_db *assetDb) {
  directLight->key = asset_direct_light_calculate_key(directLight);
  data_asset_db_insert_directLight_type_int(assetDb, directLight->key,
                                            data_int_temp(directLight->type));
  data_asset_db_insert_directLight_position_vec3(assetDb, directLight->key,
                                                 data_vec3_temp(directLight->position));
  data_asset_db_insert_directLight_innerConeAngle_float(
      assetDb, directLight->key, data_float_temp(directLight->innerConeAngle));
  data_asset_db_insert_directLight_outerConeAngle_float(
      assetDb, directLight->key, data_float_temp(directLight->outerConeAngle));
  data_asset_db_insert_directLight_intensity_float(assetDb, directLight->key,
                                                   data_float_temp(directLight->intensity));
  data_asset_db_insert_directLight_range_float(assetDb, directLight->key,
                                               data_float_temp(directLight->range));
  data_asset_db_insert_directLight_color_vec3(assetDb, directLight->key,
                                              data_vec3_temp(directLight->color));
}

void asset_direct_light_deserialize(asset_direct_light *directLight, data_asset_db *assetDb,
                                    data_key key) {
  directLight->key = key;
  directLight->type = data_asset_db_select_directLight_type_int(assetDb, directLight->key).value;
  glm_vec3_copy(data_asset_db_select_directLight_position_vec3(assetDb, directLight->key).value,
                directLight->position);
  directLight->innerConeAngle =
      data_asset_db_select_directLight_innerConeAngle_float(assetDb, directLight->key).value;
  directLight->outerConeAngle =
      data_asset_db_select_directLight_outerConeAngle_float(assetDb, directLight->key).value;
  directLight->intensity =
      data_asset_db_select_directLight_intensity_float(assetDb, directLight->key).value;
  directLight->range =
      data_asset_db_select_directLight_range_float(assetDb, directLight->key).value;
  glm_vec3_copy(data_asset_db_select_directLight_color_vec3(assetDb, directLight->key).value,
                directLight->color);
}

void asset_direct_light_debug_print(asset_direct_light *directLight, int indent) {
  log_debug(INDENT_FORMAT_STRING "directLight:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), directLight->key);
  log_debug(INDENT_FORMAT_STRING "type=%s", INDENT_FORMAT_ARGS(2),
            direct_light_type_debug_str(directLight->type));
  if (directLight->type == direct_light_type_directional) {
    log_debug(INDENT_FORMAT_STRING "direction=" VEC3_FORMAT_STRING(), INDENT_FORMAT_ARGS(2),
              VEC3_FORMAT_ARGS(directLight->direction));
  } else {
    log_debug(INDENT_FORMAT_STRING "position=" VEC3_FORMAT_STRING(), INDENT_FORMAT_ARGS(2),
              VEC3_FORMAT_ARGS(directLight->position));
  }
  if (directLight->type == direct_light_type_spot) {
    log_debug(INDENT_FORMAT_STRING "innerConeAngle=%f", INDENT_FORMAT_ARGS(2),
              directLight->innerConeAngle);
    log_debug(INDENT_FORMAT_STRING "outerConeAngle=%f", INDENT_FORMAT_ARGS(2),
              directLight->outerConeAngle);
  }
  log_debug(INDENT_FORMAT_STRING "intensity=%f", INDENT_FORMAT_ARGS(2), directLight->intensity);
  log_debug(INDENT_FORMAT_STRING "range=%f", INDENT_FORMAT_ARGS(2), directLight->range);
  log_debug(INDENT_FORMAT_STRING "color=" VEC3_FORMAT_STRING(), INDENT_FORMAT_ARGS(2),
            VEC3_FORMAT_ARGS(directLight->color));
}

asset_direct_light *asset_direct_light_create_directional_light(scene_data *sceneData,
                                                                vec3 direction, float intensity,
                                                                vec3 color) {
  asset_direct_light *directLight = core_alloc(sizeof(asset_direct_light));
  asset_direct_light_init(directLight, sceneData);

  directLight->type = direct_light_type_directional;
  glm_vec3_copy(direction, directLight->direction);
  directLight->intensity = intensity;
  glm_vec3_copy(color, directLight->color);

  assert(sceneData);
  return scene_data_add_direct_light(sceneData, directLight);
}

asset_direct_light *asset_direct_light_create_point_light(scene_data *sceneData, vec3 position,
                                                          float intensity, float range,
                                                          vec3 color) {
  asset_direct_light *directLight = core_alloc(sizeof(asset_direct_light));
  asset_direct_light_init(directLight, sceneData);

  directLight->type = direct_light_type_point;
  glm_vec3_copy(position, directLight->position);
  directLight->intensity = intensity;
  directLight->range = range;
  glm_vec3_copy(color, directLight->color);

  assert(sceneData);
  return scene_data_add_direct_light(sceneData, directLight);
}

asset_direct_light *asset_direct_light_create_spot_light(scene_data *sceneData, vec3 position,
                                                         float innerConeAngle, float outerConeAngle,
                                                         float intensity, float range, vec3 color) {
  asset_direct_light *directLight = core_alloc(sizeof(asset_direct_light));
  asset_direct_light_init(directLight, sceneData);

  directLight->type = direct_light_type_spot;
  glm_vec3_copy(position, directLight->position);
  directLight->innerConeAngle = innerConeAngle;
  directLight->outerConeAngle = outerConeAngle;
  directLight->intensity = intensity;
  directLight->range = range;
  glm_vec3_copy(color, directLight->color);

  assert(sceneData);
  return scene_data_add_direct_light(sceneData, directLight);
}
