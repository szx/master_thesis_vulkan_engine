#include "mesh.h"
#include "scene.h"

void vulkan_data_mesh_init(vulkan_data_mesh *mesh, vulkan_data_scene *sceneData) {
  mesh->sceneData = sceneData;
  utarray_alloc(mesh->primitives, sizeof(vulkan_data_primitive *));
  mesh->attributes = UnknownAttribute;
  DEF_VULKAN_ENTITY(mesh, mesh)
}

void vulkan_data_mesh_deinit(vulkan_data_mesh *mesh) { utarray_free(mesh->primitives); }

data_key vulkan_data_mesh_calculate_key(vulkan_data_mesh *mesh) {
  hash_t value;
  HASH_START(hashState)
  utarray_foreach_elem_deref (vulkan_data_primitive *, primitive, mesh->primitives) {
    HASH_UPDATE(hashState, &primitive->key.value, sizeof(primitive->key.value));
  }
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_data_mesh_serialize(vulkan_data_mesh *mesh, data_asset_db *assetDb) {
  mesh->key = vulkan_data_mesh_calculate_key(mesh);

  UT_array *primitiveKeys = NULL;
  utarray_alloc(primitiveKeys, sizeof(data_key));
  utarray_foreach_elem_deref (vulkan_data_primitive *, primitive, mesh->primitives) {
    vulkan_data_primitive_serialize(primitive, assetDb);
    utarray_push_back(primitiveKeys, &primitive->key);
  }
  data_asset_db_insert_mesh_primitives_key_array(assetDb, mesh->key,
                                                 data_key_array_temp(primitiveKeys));
  utarray_free(primitiveKeys);
}

void vulkan_data_mesh_deserialize(vulkan_data_mesh *mesh, data_asset_db *assetDb, data_key key) {
  mesh->key = key;

  data_key_array primitiveHashArray =
      data_asset_db_select_mesh_primitives_key_array(assetDb, mesh->key);
  utarray_foreach_elem_deref (data_key, primitiveKey, primitiveHashArray.values) {
    vulkan_data_primitive *primitive =
        vulkan_data_scene_get_primitive_by_key(mesh->sceneData, assetDb, primitiveKey);
    utarray_push_back(mesh->primitives, &primitive);
    mesh->attributes |= primitive->attributes;
  }

  data_key_array_deinit(&primitiveHashArray);
}

void vulkan_data_mesh_debug_print(vulkan_data_mesh *mesh, int indent) {
  log_debug(INDENT_FORMAT_STRING "mesh:", (int)INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", (int)INDENT_FORMAT_ARGS(2), mesh->key);
  utarray_foreach_elem_deref (vulkan_data_primitive *, primitive, mesh->primitives) {
    vulkan_data_primitive_debug_print(primitive, indent + 2);
  }
}
