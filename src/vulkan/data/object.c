#include "object.h"
#include "scene.h"

void vulkan_data_object_init(vulkan_data_object *object, vulkan_data_scene *sceneData) {
  object->sceneData = sceneData;
  object->mesh = NULL;
  glm_mat4_identity(object->transform);
  utarray_alloc(object->children, sizeof(vulkan_data_object *));
  data_key_init(&object->hash, 0);
  object->prev = NULL;
  object->next = NULL;
}

void vulkan_data_object_deinit(vulkan_data_object *object) {
  if (object->mesh) {
    vulkan_data_mesh_deinit(object->mesh);
    core_free(object->mesh);
  }
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
  if (object->mesh) {
    HASH_UPDATE(hashState, &object->mesh->hash, sizeof(object->mesh->hash))
  }
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void vulkan_data_object_serialize(vulkan_data_object *object, data_asset_db *assetDb) {
  object->hash = vulkan_data_object_calculate_key(object);

  if (object->mesh) {
    vulkan_data_mesh_serialize(object->mesh, assetDb);
    data_asset_db_insert_object_mesh_key(assetDb, object->hash, object->mesh->hash);
  }

  data_mat4 transformMat;
  glm_mat4_copy(object->transform, transformMat.value);
  data_asset_db_insert_object_transform_mat4(assetDb, object->hash, transformMat);

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

  if (meshHash.value != 0) {
    object->mesh = core_alloc(sizeof(vulkan_data_mesh));
    vulkan_data_mesh_init(object->mesh, object->sceneData);
    vulkan_data_mesh_deserialize(object->mesh, assetDb, meshHash);
  } else {
    log_debug("deserializing object without mesh");
  }

  data_key_array childrenHashArray =
      data_asset_db_select_object_children_key_array(assetDb, object->hash);
  data_key *childKey = NULL;
  while ((childKey = (utarray_next(childrenHashArray.values, childKey)))) {
    vulkan_data_object *child =
        vulkan_data_scene_get_object_by_key(object->sceneData, assetDb, *childKey);
    utarray_push_back(object->children, &child);
  }
  data_key_array_deinit(&childrenHashArray);
}

void vulkan_data_object_debug_print(vulkan_data_object *object, int indent) {
  log_debug("%*sobject:", indent, "");
  log_debug("%*stransform=%f %f %f %f|%f %f %f %f|%f %f %f %f|%f %f %f %f", indent + 2, "",
            object->transform[0][0], object->transform[0][1], object->transform[0][2],
            object->transform[0][3], object->transform[1][0], object->transform[1][1],
            object->transform[1][2], object->transform[1][3], object->transform[2][0],
            object->transform[2][1], object->transform[2][2], object->transform[2][3],
            object->transform[3][0], object->transform[3][1], object->transform[3][2],
            object->transform[3][3]);
  log_debug("%*shash=%zu", indent + 2, "", object->hash);

  if (object->mesh) {
    vulkan_data_mesh_debug_print(object->mesh, indent + 2);
  }

  vulkan_data_object **childIt = NULL;
  size_t i = 0;
  while ((childIt = utarray_next(object->children, childIt))) {
    vulkan_data_object *child = *childIt;
    log_debug("%*schild #%d", indent + 2, "", i++);
    vulkan_data_object_debug_print(child, indent + 2);
  }
}
