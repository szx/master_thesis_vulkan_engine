#include "mesh.h"
#include "scene.h"

void vulkan_mesh_data_init(vulkan_mesh_data *mesh, vulkan_scene_data *sceneData) {
  mesh->sceneData = sceneData;
  utarray_alloc(mesh->primitives, sizeof(vulkan_primitive_data_index));
}

void vulkan_mesh_data_deinit(vulkan_mesh_data *mesh) { utarray_free(mesh->primitives); }

data_key vulkan_mesh_data_calculate_key(vulkan_mesh_data *mesh) {
  hash_t value;
  HASH_START(hashState)
  vulkan_primitive_data_index *primitiveIdx = NULL;
  while ((primitiveIdx = (utarray_next(mesh->primitives, primitiveIdx)))) {
    vulkan_primitive_data *primitive = utarray_eltptr(mesh->sceneData->primitives, *primitiveIdx);
    HASH_UPDATE(hashState, &primitive->hash, sizeof(primitive->hash));
  }
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_mesh_data_serialize(vulkan_mesh_data *mesh, data_asset_db *assetDb) {
  mesh->hash = vulkan_mesh_data_calculate_key(mesh);
  UT_array *primitiveKeys = NULL;
  utarray_alloc(primitiveKeys, sizeof(data_key));

  vulkan_primitive_data_index *primitiveIdx = NULL;
  while ((primitiveIdx = (utarray_next(mesh->primitives, primitiveIdx)))) {
    vulkan_primitive_data *primitive = utarray_eltptr(mesh->sceneData->primitives, *primitiveIdx);
    vulkan_primitive_data_serialize(primitive, assetDb);
    utarray_push_back(primitiveKeys, &primitive->hash);
  }

  data_asset_db_insert_mesh_primitives_key_array(assetDb, mesh->hash,
                                                 data_key_array_temp(primitiveKeys));
  utarray_free(primitiveKeys);
}

void vulkan_mesh_data_deserialize(vulkan_mesh_data *mesh, data_asset_db *assetDb, data_key key) {
  mesh->hash = key;

  data_key_array primitiveHashArray =
      data_asset_db_select_mesh_primitives_key_array(assetDb, mesh->hash);

  data_key *primitiveKey = NULL;
  while ((primitiveKey = (utarray_next(primitiveHashArray.values, primitiveKey)))) {
    vulkan_primitive_data primitive;
    vulkan_primitive_data_init(&primitive);
    vulkan_primitive_data_deserialize(&primitive, assetDb, *primitiveKey);

    vulkan_primitive_data_index primitiveIdx =
        vulkan_scene_data_add_primitive(mesh->sceneData, primitive);
    utarray_push_back(mesh->primitives, &primitiveIdx);
  }
  data_key_array_deinit(&primitiveHashArray);
}

void vulkan_mesh_data_debug_print(vulkan_mesh_data *mesh) {
  log_debug("mesh:\n");
  log_debug("  hash=%zu", mesh->hash);
  vulkan_primitive_data_index *primitiveIdx = NULL;
  while ((primitiveIdx = (utarray_next(mesh->primitives, primitiveIdx)))) {
    vulkan_primitive_data *primitive = utarray_eltptr(mesh->sceneData->primitives, *primitiveIdx);
    vulkan_primitive_data_debug_print(primitive);
  }
}