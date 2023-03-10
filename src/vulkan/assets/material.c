#include "material.h"
#include "../scene/data.h"

void asset_material_init(asset_material *material, scene_data *sceneData) {
  material->sceneData = sceneData;
  // NOTE: Default material parameter values as defined by glTF spec.
  glm_vec4_copy((vec4){1.0f, 1.0f, 1.0f, 1.0f}, material->baseColorFactor);
  material->metallicFactor = 1.0f;
  material->roughnessFactor = 1.0f;
  material->baseColorTexture = NULL;
  material->metallicRoughnessTexture = NULL;
  material->normalMapTexture = NULL;

  VULKAN_ASSET_FIELD_DEFS(material, material)
}

void asset_material_deinit(asset_material *material) {}

data_key asset_material_calculate_key(asset_material *material) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &material->baseColorFactor, sizeof(material->baseColorFactor))
  HASH_UPDATE(hashState, &material->metallicFactor, sizeof(material->metallicFactor))
  HASH_UPDATE(hashState, &material->roughnessFactor, sizeof(material->roughnessFactor))
  HASH_UPDATE(hashState, &material->baseColorFactor, sizeof(material->baseColorTexture))
  if (material->baseColorTexture) {
    HASH_UPDATE(hashState, &material->baseColorTexture->key,
                sizeof(material->baseColorTexture->key))
  }
  if (material->metallicRoughnessTexture) {
    HASH_UPDATE(hashState, &material->metallicRoughnessTexture->key,
                sizeof(material->metallicRoughnessTexture->key))
  }
  if (material->normalMapTexture) {
    HASH_UPDATE(hashState, &material->normalMapTexture->key,
                sizeof(material->normalMapTexture->key))
  }
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void asset_material_serialize(asset_material *material, asset_db *assetDb) {
  material->key = asset_material_calculate_key(material);

  asset_texture_serialize(material->baseColorTexture, assetDb);
  asset_db_insert_material_baseColorTexture_key(assetDb, material->key,
                                                material->baseColorTexture->key);

  asset_texture_serialize(material->metallicRoughnessTexture, assetDb);
  asset_db_insert_material_metallicRoughnessTexture_key(assetDb, material->key,
                                                        material->metallicRoughnessTexture->key);

  asset_texture_serialize(material->normalMapTexture, assetDb);
  asset_db_insert_material_normalMapTexture_key(assetDb, material->key,
                                                material->normalMapTexture->key);

  asset_db_insert_material_baseColorFactor_vec4(assetDb, material->key,
                                                data_vec4_temp(material->baseColorFactor));
  asset_db_insert_material_metallicFactor_float(assetDb, material->key,
                                                data_float_temp(material->metallicFactor));
  asset_db_insert_material_roughnessFactor_float(assetDb, material->key,
                                                 data_float_temp(material->roughnessFactor));
}

void asset_material_deserialize(asset_material *material, asset_db *assetDb, data_key key) {
  material->key = key;
  glm_vec4_copy(asset_db_select_material_baseColorFactor_vec4(assetDb, material->key).value,
                material->baseColorFactor);
  material->metallicFactor =
      asset_db_select_material_metallicFactor_float(assetDb, material->key).value;
  material->roughnessFactor =
      asset_db_select_material_roughnessFactor_float(assetDb, material->key).value;

  material->baseColorTexture = scene_data_get_texture_by_key(
      material->sceneData, assetDb,
      asset_db_select_material_baseColorTexture_key(assetDb, material->key));
  material->metallicRoughnessTexture = scene_data_get_texture_by_key(
      material->sceneData, assetDb,
      asset_db_select_material_metallicRoughnessTexture_key(assetDb, material->key));
  material->normalMapTexture = scene_data_get_texture_by_key(
      material->sceneData, assetDb,
      asset_db_select_material_normalMapTexture_key(assetDb, material->key));
}

void asset_material_debug_print(asset_material *material, int indent) {
  log_debug(INDENT_FORMAT_STRING "material:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu:", INDENT_FORMAT_ARGS(2), material->key);
  log_debug(INDENT_FORMAT_STRING "baseColorFactor=%f %f %f %f", INDENT_FORMAT_ARGS(2),
            material->baseColorFactor[0], material->baseColorFactor[1],
            material->baseColorFactor[2], material->baseColorFactor[3]);
  log_debug(INDENT_FORMAT_STRING "metallicFactor=%zu", INDENT_FORMAT_ARGS(2),
            material->metallicFactor);
  log_debug(INDENT_FORMAT_STRING "roughnessFactor=%zu", INDENT_FORMAT_ARGS(2),
            material->roughnessFactor);
}
