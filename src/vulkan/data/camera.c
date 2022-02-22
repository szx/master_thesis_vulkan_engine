#include "camera.h"

void vulkan_data_camera_init(vulkan_data_camera *camera, vulkan_data_scene *sceneData) {
  glm_vec3((vec3){0.0f, 0.0f, -1.0f}, camera->position);
  glm_quat_identity(camera->rotation);
  camera->fovY = 90.0f;
  camera->aspectRatio = 1.0f;
  camera->nearZ = 0.1f;
  camera->farZ = 256.0f;
  camera->dirty = true;
  DEF_VULKAN_ENTITY(camera, camera)
}

void vulkan_data_camera_deinit(vulkan_data_camera *camera) {}

void vulkan_data_camera_copy(vulkan_data_camera *dst, vulkan_data_camera *src) {
  glm_vec3_copy(src->position, dst->position);
  glm_vec4_copy(src->rotation, dst->rotation);
  dst->fovY = src->fovY;
  dst->aspectRatio = src->aspectRatio;
  dst->nearZ = src->nearZ;
  dst->farZ = src->farZ;
  dst->dirty = true;
}

void vulkan_data_camera_update_aspect_ratio(vulkan_data_camera *camera, float aspectRatio) {
  camera->aspectRatio = aspectRatio;
  camera->dirty = true;
}

data_key vulkan_data_camera_calculate_key(vulkan_data_camera *camera) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, camera->position, sizeof(camera->position))
  HASH_UPDATE(hashState, camera->rotation, sizeof(camera->rotation))
  HASH_UPDATE(hashState, &camera->fovY, sizeof(camera->fovY))
  HASH_UPDATE(hashState, &camera->aspectRatio, sizeof(camera->aspectRatio))
  HASH_UPDATE(hashState, &camera->nearZ, sizeof(camera->nearZ))
  HASH_UPDATE(hashState, &camera->farZ, sizeof(camera->farZ))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState);
  return (data_key){value};
}

void vulkan_data_camera_serialize(vulkan_data_camera *camera, data_asset_db *assetDb) {
  camera->key = vulkan_data_camera_calculate_key(camera);
  data_asset_db_insert_camera_position_vec3(assetDb, camera->key, data_vec3_temp(camera->position));
  data_asset_db_insert_camera_rotation_vec4(assetDb, camera->key, data_vec4_temp(camera->rotation));
  data_asset_db_insert_camera_fovY_float(assetDb, camera->key, data_float_temp(camera->fovY));
  data_asset_db_insert_camera_aspectRatio_float(assetDb, camera->key,
                                                data_float_temp(camera->aspectRatio));
  data_asset_db_insert_camera_nearZ_float(assetDb, camera->key, data_float_temp(camera->nearZ));
  data_asset_db_insert_camera_farZ_float(assetDb, camera->key, data_float_temp(camera->farZ));
}

void vulkan_data_camera_deserialize(vulkan_data_camera *camera, data_asset_db *assetDb,
                                    data_key key) {
  camera->key = key;
  glm_vec3_copy(data_asset_db_select_camera_position_vec3(assetDb, camera->key).value,
                camera->position);
  glm_vec4_copy(data_asset_db_select_camera_rotation_vec4(assetDb, camera->key).value,
                camera->rotation);
  camera->fovY = data_asset_db_select_camera_fovY_float(assetDb, camera->key).value;
  camera->aspectRatio = data_asset_db_select_camera_aspectRatio_float(assetDb, camera->key).value;
  camera->nearZ = data_asset_db_select_camera_nearZ_float(assetDb, camera->key).value;
  data_float d = data_asset_db_select_camera_farZ_float(assetDb, camera->key);
  camera->farZ = d.value;
}
