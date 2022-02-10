#include "mesh.h"
#include "scene.h"

void vulkan_data_mesh_init(vulkan_data_mesh *mesh, vulkan_data_scene *sceneData) {
  mesh->sceneData = sceneData;
  utarray_alloc(mesh->primitives, sizeof(vulkan_data_primitive *));
  mesh->graphNode = NULL;
}

void vulkan_data_mesh_deinit(vulkan_data_mesh *mesh) { utarray_free(mesh->primitives); }

data_key vulkan_data_mesh_calculate_key(vulkan_data_mesh *mesh) {
  hash_t value;
  HASH_START(hashState)
  vulkan_data_primitive **primitiveIt = NULL;
  while ((primitiveIt = (utarray_next(mesh->primitives, primitiveIt)))) {
    vulkan_data_primitive *primitive = *primitiveIt;
    HASH_UPDATE(hashState, &primitive->hash.value, sizeof(primitive->hash.value));
  }
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_data_mesh_serialize(vulkan_data_mesh *mesh, data_asset_db *assetDb) {
  mesh->hash = vulkan_data_mesh_calculate_key(mesh);

  UT_array *primitiveKeys = NULL;
  utarray_alloc(primitiveKeys, sizeof(data_key));
  vulkan_data_primitive **primitiveIt = NULL;
  while ((primitiveIt = (utarray_next(mesh->primitives, primitiveIt)))) {
    vulkan_data_primitive *primitive = *primitiveIt;
    vulkan_data_primitive_serialize(primitive, assetDb);
    utarray_push_back(primitiveKeys, &primitive->hash);
  }
  data_asset_db_insert_mesh_primitives_key_array(assetDb, mesh->hash,
                                                 data_key_array_temp(primitiveKeys));
  utarray_free(primitiveKeys);
}

void vulkan_data_mesh_deserialize(vulkan_data_mesh *mesh, data_asset_db *assetDb, data_key key) {
  mesh->hash = key;

  data_key_array primitiveHashArray =
      data_asset_db_select_mesh_primitives_key_array(assetDb, mesh->hash);
  data_key *primitiveKey = NULL;
  while ((primitiveKey = (utarray_next(primitiveHashArray.values, primitiveKey)))) {
    vulkan_data_primitive *primitive =
        vulkan_data_scene_get_primitive_by_key(mesh->sceneData, assetDb, *primitiveKey);
    utarray_push_back(mesh->primitives, &primitive);
  }
  data_key_array_deinit(&primitiveHashArray);
}

void vulkan_data_mesh_debug_print(vulkan_data_mesh *mesh, int indent) {
  log_debug("%*smesh:", (int)indent, "");
  log_debug("%*shash=%zu", (int)indent + 2, "", mesh->hash);
  vulkan_data_primitive **primitiveIt = NULL;
  while ((primitiveIt = (utarray_next(mesh->primitives, primitiveIt)))) {
    vulkan_data_primitive *primitive = *primitiveIt;
    vulkan_data_primitive_debug_print(primitive, indent + 2);
  }
}
