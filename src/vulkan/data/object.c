#include "object.h"
#include "scene.h"

void vulkan_data_object_init(vulkan_data_object *object, vulkan_data_scene *sceneData) {
  object->sceneData = sceneData;
  glm_mat4_identity(object->transform);
  utarray_alloc(object->children, sizeof(vulkan_data_object *));
  data_key_init(&object->hash, 0);
}

void vulkan_data_object_deinit(vulkan_data_object *object) {
  vulkan_data_mesh_deinit(&object->mesh);
  utarray_free(object->children);
}

data_key vulkan_data_object_calculate_key(vulkan_data_object *object) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &object->transform, sizeof(object->transform))
  vulkan_data_object **childIt = NULL;
  while ((childIt = (utarray_next(object->children, childIt)))) {
    vulkan_data_object *child = *childIt;
    HASH_UPDATE(hashState, &child->hash.value, sizeof(child->hash.value));
  }
  HASH_UPDATE(hashState, &object->mesh.hash, sizeof(object->mesh.hash))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_data_object_serialize(vulkan_data_object *object, data_asset_db *assetDb) {
  object->hash = vulkan_data_object_calculate_key(object);
  vulkan_data_mesh *mesh = &object->mesh;
  vulkan_data_mesh_serialize(mesh, assetDb);

  data_mat4 transformMat;
  glm_mat4_copy(object->transform, transformMat.value);
  data_asset_db_insert_object_transform_mat4(assetDb, object->hash, transformMat);

  data_asset_db_insert_object_mesh_key(assetDb, object->hash, object->mesh.hash);

  UT_array *childKeys = NULL;
  utarray_alloc(childKeys, sizeof(data_key));
  vulkan_data_object **childIt = NULL;
  while ((childIt = (utarray_next(object->children, childIt)))) {
    vulkan_data_object *child = *childIt;
    vulkan_data_object_serialize(child, assetDb);
    utarray_push_back(childKeys, &child->hash);
  }
  data_asset_db_insert_object_children_key_array(assetDb, object->hash,
                                                 data_key_array_temp(childKeys));
  utarray_free(childKeys);
}

void vulkan_data_object_deserialize(vulkan_data_object *object, data_asset_db *assetDb,
                                    data_key key) {
  object->hash = key;

  glm_mat4_copy(data_asset_db_select_object_transform_mat4(assetDb, object->hash).value,
                object->transform);

  data_key meshHash = data_asset_db_select_object_mesh_key(assetDb, object->hash);
  vulkan_data_mesh_init(&object->mesh, object->sceneData);
  vulkan_data_mesh_deserialize(&object->mesh, assetDb, meshHash);
}

void vulkan_data_object_debug_print(vulkan_data_object *object) {
  log_debug("object:\n");
  glm_mat4_print(object->transform, stderr);
  log_debug("  hash=%zu", object->hash);
  vulkan_data_mesh *mesh = &object->mesh;
  vulkan_data_mesh_debug_print(mesh);
}
