#include "mesh.h"
#include "../scene/data.h"

void asset_mesh_init(asset_mesh *mesh, scene_data *sceneData) {
  mesh->sceneData = sceneData;
  utarray_alloc(mesh->primitives, sizeof(asset_primitive *));
  VULKAN_ASSET_FIELD_DEFS(mesh, mesh)
}

void asset_mesh_deinit(asset_mesh *mesh) { utarray_free(mesh->primitives); }

data_key asset_mesh_calculate_key(asset_mesh *mesh) {
  hash_t value;
  HASH_START(hashState)
  utarray_foreach_elem_deref (asset_primitive *, primitive, mesh->primitives) {
    HASH_UPDATE(hashState, &primitive->key.value, sizeof(primitive->key.value));
  }
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void asset_mesh_serialize(asset_mesh *mesh, asset_db *assetDb) {
  mesh->key = asset_mesh_calculate_key(mesh);

  UT_array *primitiveKeys = NULL;
  utarray_alloc(primitiveKeys, sizeof(data_key));
  utarray_foreach_elem_deref (asset_primitive *, primitive, mesh->primitives) {
    asset_primitive_serialize(primitive, assetDb);
    assert(primitive->vertexCount > 0);
    utarray_push_back(primitiveKeys, &primitive->key);
  }
  asset_db_insert_mesh_primitives_key_array(assetDb, mesh->key, data_key_array_temp(primitiveKeys));
  utarray_free(primitiveKeys);
}

void asset_mesh_deserialize(asset_mesh *mesh, asset_db *assetDb, data_key key) {
  mesh->key = key;

  data_key_array primitiveHashArray = asset_db_select_mesh_primitives_key_array(assetDb, mesh->key);
  utarray_foreach_elem_deref (data_key, primitiveKey, primitiveHashArray.values) {
    asset_primitive *primitive =
        scene_data_get_primitive_by_key(mesh->sceneData, assetDb, primitiveKey);
    assert(primitive->vertexCount > 0);
    utarray_push_back(mesh->primitives, &primitive);
  }

  data_key_array_deinit(&primitiveHashArray);
}

void asset_mesh_debug_print(asset_mesh *mesh, int indent) {
  log_debug(INDENT_FORMAT_STRING "mesh:", (int)INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", (int)INDENT_FORMAT_ARGS(2), mesh->key);
  utarray_foreach_elem_deref (asset_primitive *, primitive, mesh->primitives) {
    asset_primitive_debug_print(primitive, indent + 2);
  }
}
