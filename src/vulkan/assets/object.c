#include "object.h"
#include "../scene/data.h"

void asset_object_init(asset_object *object, scene_data *sceneData) {
  object->sceneData = sceneData;
  object->mesh = NULL;
  object->camera = NULL;
  glm_mat4_identity(object->transform);
  utarray_alloc(object->children, sizeof(asset_object *));

  VULKAN_ASSET_FIELD_DEFS(object, object)
}

void asset_object_deinit(asset_object *object) {
  if (object->mesh) {
    asset_mesh_deinit(object->mesh);
    core_free(object->mesh);
  }
  if (object->camera) {
    asset_camera_deinit(object->camera);
    core_free(object->camera);
  }
  utarray_free(object->children);
}

data_key asset_object_calculate_key(asset_object *object) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &object->transform, sizeof(object->transform))

  utarray_foreach_elem_deref (asset_object *, child, object->children) {
    HASH_UPDATE(hashState, &child->key.value, sizeof(child->key.value));
  }
  if (object->mesh) {
    HASH_UPDATE(hashState, &object->mesh->key, sizeof(object->mesh->key))
  }
  if (object->camera) {
    HASH_UPDATE(hashState, &object->camera->key, sizeof(object->camera->key))
  }
  HASH_DIGEST(hashState, value)
  HASH_END(hashState)
  return (data_key){value};
}

void asset_object_serialize(asset_object *object, data_asset_db *assetDb) {
  object->key = asset_object_calculate_key(object);

  if (object->mesh) {
    asset_mesh_serialize(object->mesh, assetDb);
    data_asset_db_insert_object_mesh_key(assetDb, object->key, object->mesh->key);
  }

  if (object->camera) {
    asset_camera_serialize(object->camera, assetDb);
    data_asset_db_insert_object_camera_key(assetDb, object->key, object->camera->key);
  }

  data_mat4 transformMat;
  glm_mat4_copy(object->transform, transformMat.value);
  data_asset_db_insert_object_transform_mat4(assetDb, object->key, transformMat);

  UT_array *childKeys = NULL;
  utarray_alloc(childKeys, sizeof(data_key));
  utarray_foreach_elem_deref (asset_object *, child, object->children) {
    asset_object_serialize(child, assetDb);
    utarray_push_back(childKeys, &child->key);
  }
  data_asset_db_insert_object_children_key_array(assetDb, object->key,
                                                 data_key_array_temp(childKeys));
  utarray_free(childKeys);
}

void asset_object_deserialize(asset_object *object, data_asset_db *assetDb, data_key key) {
  object->key = key;

  glm_mat4_copy(data_asset_db_select_object_transform_mat4(assetDb, object->key).value,
                object->transform);

  data_key meshHash = data_asset_db_select_object_mesh_key(assetDb, object->key);
  if (meshHash.value != 0) {
    object->mesh = core_alloc(sizeof(asset_mesh));
    asset_mesh_init(object->mesh, object->sceneData);
    asset_mesh_deserialize(object->mesh, assetDb, meshHash);
  } else {
    log_debug("deserializing object without mesh");
  }

  data_key cameraHash = data_asset_db_select_object_camera_key(assetDb, object->key);
  if (cameraHash.value != 0) {
    object->camera = core_alloc(sizeof(asset_camera));
    asset_camera_init(object->camera, object->sceneData);
    asset_camera_deserialize(object->camera, assetDb, cameraHash);
  } else {
    log_debug("deserializing object without camera");
  }

  data_key_array childrenHashArray =
      data_asset_db_select_object_children_key_array(assetDb, object->key);
  utarray_foreach_elem_deref (data_key, childKey, childrenHashArray.values) {
    asset_object *child = scene_data_get_object_by_key(object->sceneData, assetDb, childKey);
    utarray_push_back(object->children, &child);
  }
  data_key_array_deinit(&childrenHashArray);
}

void asset_object_debug_print(asset_object *object, int indent) {
  log_debug(INDENT_FORMAT_STRING "object:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "transform=" MAT4_FORMAT_STRING(" | "), INDENT_FORMAT_ARGS(2),
            MAT4_FORMAT_ARGS(object->transform));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), object->key);

  if (object->mesh) {
    asset_mesh_debug_print(object->mesh, indent + 2);
  }

  if (object->camera) {
    asset_camera_debug_print(object->camera, indent + 2);
  }

  size_t i = 0;
  utarray_foreach_elem_deref (asset_object *, child, object->children) {
    log_debug(INDENT_FORMAT_STRING "child #%d", INDENT_FORMAT_ARGS(2), i++);
    asset_object_debug_print(child, indent + 2);
  }
}
