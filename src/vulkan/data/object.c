#include "object.h"
#include "scene.h"

void vulkan_data_object_init(vulkan_data_object *object, vulkan_data_scene *sceneData) {
  object->sceneData = sceneData;
  object->mesh = NULL;
  object->camera = NULL;
  glm_mat4_identity(object->transform);
  utarray_alloc(object->children, sizeof(vulkan_data_object *));

  DEF_VULKAN_ENTITY(object, object)
}

void vulkan_data_object_deinit(vulkan_data_object *object) {
  if (object->mesh) {
    vulkan_data_mesh_deinit(object->mesh);
    core_free(object->mesh);
  }
  if (object->camera) {
    vulkan_data_camera_deinit(object->camera);
    core_free(object->camera);
  }
  utarray_free(object->children);
}

data_key vulkan_data_object_calculate_key(vulkan_data_object *object) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &object->transform, sizeof(object->transform))

  utarray_foreach_elem_deref (vulkan_data_object *, child, object->children) {
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

void vulkan_data_object_serialize(vulkan_data_object *object, data_asset_db *assetDb) {
  object->key = vulkan_data_object_calculate_key(object);

  if (object->mesh) {
    vulkan_data_mesh_serialize(object->mesh, assetDb);
    data_asset_db_insert_object_mesh_key(assetDb, object->key, object->mesh->key);
  }

  if (object->camera) {
    vulkan_data_camera_serialize(object->camera, assetDb);
    data_asset_db_insert_object_camera_key(assetDb, object->key, object->camera->key);
  }

  data_mat4 transformMat;
  glm_mat4_copy(object->transform, transformMat.value);
  data_asset_db_insert_object_transform_mat4(assetDb, object->key, transformMat);

  UT_array *childKeys = NULL;
  utarray_alloc(childKeys, sizeof(data_key));
  utarray_foreach_elem_deref (vulkan_data_object *, child, object->children) {
    vulkan_data_object_serialize(child, assetDb);
    utarray_push_back(childKeys, &child->key);
  }
  data_asset_db_insert_object_children_key_array(assetDb, object->key,
                                                 data_key_array_temp(childKeys));
  utarray_free(childKeys);
}

void vulkan_data_object_deserialize(vulkan_data_object *object, data_asset_db *assetDb,
                                    data_key key) {
  object->key = key;

  glm_mat4_copy(data_asset_db_select_object_transform_mat4(assetDb, object->key).value,
                object->transform);

  data_key meshHash = data_asset_db_select_object_mesh_key(assetDb, object->key);
  if (meshHash.value != 0) {
    object->mesh = core_alloc(sizeof(vulkan_data_mesh));
    vulkan_data_mesh_init(object->mesh, object->sceneData);
    vulkan_data_mesh_deserialize(object->mesh, assetDb, meshHash);
  } else {
    log_debug("deserializing object without mesh");
  }

  data_key cameraHash = data_asset_db_select_object_camera_key(assetDb, object->key);
  if (cameraHash.value != 0) {
    object->camera = core_alloc(sizeof(vulkan_data_camera));
    vulkan_data_camera_init(object->camera, object->sceneData);
    vulkan_data_camera_deserialize(object->camera, assetDb, cameraHash);
  } else {
    log_debug("deserializing object without camera");
  }

  data_key_array childrenHashArray =
      data_asset_db_select_object_children_key_array(assetDb, object->key);
  utarray_foreach_elem_deref (data_key, childKey, childrenHashArray.values) {
    vulkan_data_object *child =
        vulkan_data_scene_get_object_by_key(object->sceneData, assetDb, childKey);
    utarray_push_back(object->children, &child);
  }
  data_key_array_deinit(&childrenHashArray);
}

void vulkan_data_object_debug_print(vulkan_data_object *object, int indent) {
  log_debug(INDENT_FORMAT_STRING "object:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "transform=" MAT4_FORMAT_STRING(" | "), INDENT_FORMAT_ARGS(2),
            MAT4_FORMAT_ARGS(object->transform));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), object->key);

  if (object->mesh) {
    vulkan_data_mesh_debug_print(object->mesh, indent + 2);
  }

  if (object->camera) {
    vulkan_data_camera_debug_print(object->camera, indent + 2);
  }

  size_t i = 0;
  utarray_foreach_elem_deref (vulkan_data_object *, child, object->children) {
    log_debug(INDENT_FORMAT_STRING "child #%d", INDENT_FORMAT_ARGS(2), i++);
    vulkan_data_object_debug_print(child, indent + 2);
  }
}
