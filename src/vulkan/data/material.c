#include "material.h"
#include "scene.h"

void vulkan_data_material_init(vulkan_data_material *material, vulkan_data_scene *sceneData) {
  material->sceneData = sceneData;
  glm_vec4_copy((vec4){1.0f, 1.0f, 1.0f, 1.0f}, material->baseColorFactor);
  material->metallicFactor = 0.5f;
  material->roughnessFactor = 0.1f;
  material->baseColorTexture = NULL;
  material->metallicRoughnessTexture = NULL;

  data_key_init(&material->hash, 0);
  material->prev = NULL;
  material->next = NULL;
  material->graphNode = NULL;
}

void vulkan_data_material_deinit(vulkan_data_material *material) {}

data_key vulkan_data_material_calculate_key(vulkan_data_material *material) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &material->baseColorFactor, sizeof(material->baseColorFactor))
  HASH_UPDATE(hashState, &material->metallicFactor, sizeof(material->metallicFactor))
  HASH_UPDATE(hashState, &material->roughnessFactor, sizeof(material->roughnessFactor))
  HASH_UPDATE(hashState, &material->baseColorFactor, sizeof(material->baseColorTexture))
  HASH_UPDATE(hashState, &material->baseColorTexture->hash,
              sizeof(material->baseColorTexture->hash))
  HASH_UPDATE(hashState, &material->metallicRoughnessTexture->hash,
              sizeof(material->metallicRoughnessTexture->hash))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_data_material_serialize(vulkan_data_material *material, data_asset_db *assetDb) {
  material->hash = vulkan_data_material_calculate_key(material);

  vulkan_data_texture_serialize(material->baseColorTexture, assetDb);
  data_asset_db_insert_material_baseColorTexture_key(assetDb, material->hash,
                                                     material->baseColorTexture->hash);

  vulkan_data_texture_serialize(material->metallicRoughnessTexture, assetDb);
  data_asset_db_insert_material_metallicRoughnessTexture_key(
      assetDb, material->hash, material->metallicRoughnessTexture->hash);

  data_asset_db_insert_material_baseColorFactor_vec4(assetDb, material->hash,
                                                     data_vec4_temp(material->baseColorFactor));
  data_asset_db_insert_material_metallicFactor_float(assetDb, material->hash,
                                                     data_float_temp(material->metallicFactor));
  data_asset_db_insert_material_roughnessFactor_float(assetDb, material->hash,
                                                      data_float_temp(material->roughnessFactor));
}

void vulkan_data_material_deserialize(vulkan_data_material *material, data_asset_db *assetDb,
                                      data_key key) {
  material->hash = key;
  glm_vec4_copy(data_asset_db_select_material_baseColorFactor_vec4(assetDb, material->hash).value,
                material->baseColorFactor);
  material->metallicFactor =
      data_asset_db_select_material_metallicFactor_float(assetDb, material->hash).value;
  material->roughnessFactor =
      data_asset_db_select_material_roughnessFactor_float(assetDb, material->hash).value;

  material->baseColorTexture = vulkan_data_scene_get_texture_by_key(
      material->sceneData, assetDb,
      data_asset_db_select_material_baseColorTexture_key(assetDb, material->hash));
  material->metallicRoughnessTexture = vulkan_data_scene_get_texture_by_key(
      material->sceneData, assetDb,
      data_asset_db_select_material_metallicRoughnessTexture_key(assetDb, material->hash));
}

void vulkan_data_material_debug_print(vulkan_data_material *material) {
  log_debug("material:");
  log_debug("  hash=%zu", material->hash);
  log_debug("  baseColorFactor=%f %f %f %f", material->baseColorFactor[0],
            material->baseColorFactor[1], material->baseColorFactor[2],
            material->baseColorFactor[3]);
  log_debug("  metallicFactor=%zu", material->metallicFactor);
  log_debug("  roughnessFactor=%zu", material->roughnessFactor);
}
