#include "camera.h"

void vulkan_camera_data_init(vulkan_camera_data *camera) {
  glm_vec3((vec3){0.0f, 0.0f, -1.0f}, camera->position);
  glm_quat_identity(camera->rotation);
  camera->fovY = 90.0f;
  camera->aspectRatio = 1.0f;
  camera->nearZ = 0.1f;
  camera->farZ = 256.0f;
  camera->dirty = true;
  camera->hash = vulkan_camera_data_calculate_key(camera);
}

void vulkan_camera_data_deinit(vulkan_camera_data *camera) {}

void vulkan_camera_data_update_aspect_ratio(vulkan_camera_data *camera, float aspectRatio) {
  camera->aspectRatio = aspectRatio;
  camera->dirty = true;
}

data_key vulkan_camera_data_calculate_key(vulkan_camera_data *camera) {
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

void vulkan_camera_data_serialize(vulkan_camera_data *camera, data_asset_db *assetDb) {
  camera->hash = vulkan_camera_data_calculate_key(camera);
  data_asset_db_insert_camera_position_vec3(assetDb, camera->hash,
                                            data_vec3_temp(camera->position));
  data_asset_db_insert_camera_rotation_vec4(assetDb, camera->hash,
                                            data_vec4_temp(camera->rotation));
  data_asset_db_insert_camera_fovY_float(assetDb, camera->hash, data_float_temp(camera->fovY));
  data_asset_db_insert_camera_aspectRatio_float(assetDb, camera->hash,
                                                data_float_temp(camera->aspectRatio));
  data_asset_db_insert_camera_nearZ_float(assetDb, camera->hash, data_float_temp(camera->nearZ));
  data_asset_db_insert_camera_farZ_float(assetDb, camera->hash, data_float_temp(camera->farZ));
}

void vulkan_camera_data_deserialize(vulkan_camera_data *camera, data_asset_db *assetDb,
                                    data_key key) {
  camera->hash = key;
  glm_vec3_copy(data_asset_db_select_camera_position_vec3(assetDb, camera->hash).value,
                camera->position);
  glm_vec4_copy(data_asset_db_select_camera_rotation_vec4(assetDb, camera->hash).value,
                camera->rotation);
  camera->fovY = data_asset_db_select_camera_fovY_float(assetDb, camera->hash).value;
  camera->aspectRatio = data_asset_db_select_camera_aspectRatio_float(assetDb, camera->hash).value;
  camera->nearZ = data_asset_db_select_camera_nearZ_float(assetDb, camera->hash).value;
  data_float d = data_asset_db_select_camera_farZ_float(assetDb, camera->hash);
  camera->farZ = d.value;
}