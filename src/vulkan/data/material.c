#include "material.h"

void vulkan_material_data_init(vulkan_material_data *material) {
  glm_vec4_copy((vec4){1.0f, 1.0f, 1.0f, 1.0f}, material->baseColorFactor);
  material->metallicFactor = 0.5f;
  material->roughnessFactor = 0.1f;
  material->hash = vulkan_material_data_calculate_key(material);
  material->prev = NULL;
  material->next = NULL;
}

void vulkan_material_data_deinit(vulkan_material_data *material) {}

data_key vulkan_material_data_calculate_key(vulkan_material_data *material) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &material->baseColorFactor, sizeof(material->baseColorFactor))
  HASH_UPDATE(hashState, &material->metallicFactor, sizeof(material->metallicFactor))
  HASH_UPDATE(hashState, &material->roughnessFactor, sizeof(material->roughnessFactor))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_material_data_serialize(vulkan_material_data *material, data_asset_db *assetDb) {
  material->hash = vulkan_material_data_calculate_key(material);
  data_asset_db_insert_material_baseColorFactor_vec4(assetDb, material->hash,
                                                     data_vec4_temp(material->baseColorFactor));
  data_asset_db_insert_material_metallicFactor_float(assetDb, material->hash,
                                                     data_float_temp(material->metallicFactor));
  data_asset_db_insert_material_roughnessFactor_float(assetDb, material->hash,
                                                      data_float_temp(material->roughnessFactor));
}

void vulkan_material_data_deserialize(vulkan_material_data *material, data_asset_db *assetDb,
                                      data_key key) {
  material->hash = key;
  glm_vec4_copy(data_asset_db_select_material_baseColorFactor_vec4(assetDb, material->hash).value,
                material->baseColorFactor);
  material->metallicFactor =
      data_asset_db_select_material_metallicFactor_float(assetDb, material->hash).value;
  material->roughnessFactor =
      data_asset_db_select_material_roughnessFactor_float(assetDb, material->hash).value;
}

void vulkan_material_data_debug_print(vulkan_material_data *material) {
  log_debug("material:");
  log_debug("  hash=%zu", material->hash);
  log_debug("  baseColorFactor=%f %f %f %f", material->baseColorFactor[0],
            material->baseColorFactor[1], material->baseColorFactor[2],
            material->baseColorFactor[3]);
  log_debug("  metallicFactor=%zu", material->metallicFactor);
  log_debug("  roughnessFactor=%zu", material->roughnessFactor);
}
