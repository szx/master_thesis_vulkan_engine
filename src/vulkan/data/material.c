#include "material.h"
#include "scene.h"

void vulkan_data_material_init(vulkan_data_material *material, vulkan_data_scene *sceneData) {
  material->sceneData = sceneData;
  // NOTE: Default material parameter values as defined by glTF spec.
  glm_vec4_copy((vec4){1.0f, 1.0f, 1.0f, 1.0f}, material->baseColorFactor);
  material->metallicFactor = 1.0f;
  material->roughnessFactor = 1.0f;
  material->baseColorTexture = NULL;
  material->metallicRoughnessTexture = NULL;

  DEF_VULKAN_ENTITY(material, material)
}

void vulkan_data_material_deinit(vulkan_data_material *material) {}

data_key vulkan_data_material_calculate_key(vulkan_data_material *material) {
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
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_data_material_serialize(vulkan_data_material *material, data_asset_db *assetDb) {
  material->key = vulkan_data_material_calculate_key(material);

  vulkan_data_texture_serialize(material->baseColorTexture, assetDb);
  data_asset_db_insert_material_baseColorTexture_key(assetDb, material->key,
                                                     material->baseColorTexture->key);

  vulkan_data_texture_serialize(material->metallicRoughnessTexture, assetDb);
  data_asset_db_insert_material_metallicRoughnessTexture_key(
      assetDb, material->key, material->metallicRoughnessTexture->key);

  data_asset_db_insert_material_baseColorFactor_vec4(assetDb, material->key,
                                                     data_vec4_temp(material->baseColorFactor));
  data_asset_db_insert_material_metallicFactor_float(assetDb, material->key,
                                                     data_float_temp(material->metallicFactor));
  data_asset_db_insert_material_roughnessFactor_float(assetDb, material->key,
                                                      data_float_temp(material->roughnessFactor));
}

void vulkan_data_material_deserialize(vulkan_data_material *material, data_asset_db *assetDb,
                                      data_key key) {
  material->key = key;
  glm_vec4_copy(data_asset_db_select_material_baseColorFactor_vec4(assetDb, material->key).value,
                material->baseColorFactor);
  material->metallicFactor =
      data_asset_db_select_material_metallicFactor_float(assetDb, material->key).value;
  material->roughnessFactor =
      data_asset_db_select_material_roughnessFactor_float(assetDb, material->key).value;

  material->baseColorTexture = vulkan_data_scene_get_texture_by_key(
      material->sceneData, assetDb,
      data_asset_db_select_material_baseColorTexture_key(assetDb, material->key));
  material->metallicRoughnessTexture = vulkan_data_scene_get_texture_by_key(
      material->sceneData, assetDb,
      data_asset_db_select_material_metallicRoughnessTexture_key(assetDb, material->key));
}

void vulkan_data_material_debug_print(vulkan_data_material *material) {
  log_debug("material:");
  log_debug("  hash=%zu", material->key);
  log_debug("  baseColorFactor=%f %f %f %f", material->baseColorFactor[0],
            material->baseColorFactor[1], material->baseColorFactor[2],
            material->baseColorFactor[3]);
  log_debug("  metallicFactor=%zu", material->metallicFactor);
  log_debug("  roughnessFactor=%zu", material->roughnessFactor);
}
