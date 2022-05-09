#include "camera.h"

void vulkan_asset_camera_init(vulkan_asset_camera *camera, vulkan_scene_data *sceneData) {
  camera->type = vulkan_camera_type_perspective;
  camera->fovY = glm_rad(45.0f);
  camera->aspectRatio = 1.0f;
  camera->nearZ = 0.1f;
  camera->farZ = 256.0f;
  camera->dirty = true;
  VULKAN_ASSET_FIELD_DEFS(camera, camera)
}

void vulkan_asset_camera_deinit(vulkan_asset_camera *camera) {}

void vulkan_asset_camera_copy(vulkan_asset_camera *dst, vulkan_asset_camera *src) {
  dst->type = src->type;
  dst->fovY = src->fovY;
  dst->aspectRatio = src->aspectRatio;
  dst->nearZ = src->nearZ;
  dst->farZ = src->farZ;
  dst->dirty = true;
}

data_key vulkan_asset_camera_calculate_key(vulkan_asset_camera *camera) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, &camera->type, sizeof(camera->type))
  HASH_UPDATE(hashState, &camera->fovY, sizeof(camera->fovY))
  HASH_UPDATE(hashState, &camera->aspectRatio, sizeof(camera->aspectRatio))
  HASH_UPDATE(hashState, &camera->nearZ, sizeof(camera->nearZ))
  HASH_UPDATE(hashState, &camera->farZ, sizeof(camera->farZ))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState);
  return (data_key){value};
}

void vulkan_asset_camera_serialize(vulkan_asset_camera *camera, data_asset_db *assetDb) {
  camera->key = vulkan_asset_camera_calculate_key(camera);
  data_asset_db_insert_camera_type_int(assetDb, camera->key, data_int_temp(camera->type));
  data_asset_db_insert_camera_fovY_float(assetDb, camera->key, data_float_temp(camera->fovY));
  data_asset_db_insert_camera_aspectRatio_float(assetDb, camera->key,
                                                data_float_temp(camera->aspectRatio));
  data_asset_db_insert_camera_nearZ_float(assetDb, camera->key, data_float_temp(camera->nearZ));
  data_asset_db_insert_camera_farZ_float(assetDb, camera->key, data_float_temp(camera->farZ));
}

void vulkan_asset_camera_deserialize(vulkan_asset_camera *camera, data_asset_db *assetDb,
                                     data_key key) {
  camera->key = key;
  camera->type = data_asset_db_select_camera_type_int(assetDb, camera->key).value;
  camera->fovY = data_asset_db_select_camera_fovY_float(assetDb, camera->key).value;
  camera->aspectRatio = data_asset_db_select_camera_aspectRatio_float(assetDb, camera->key).value;
  camera->nearZ = data_asset_db_select_camera_nearZ_float(assetDb, camera->key).value;
  data_float d = data_asset_db_select_camera_farZ_float(assetDb, camera->key);
  camera->farZ = d.value;
}

void vulkan_asset_camera_debug_print(vulkan_asset_camera *camera, int indent) {
  log_debug(INDENT_FORMAT_STRING "camera:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "hash=%zu", INDENT_FORMAT_ARGS(2), camera->key);
  log_debug(INDENT_FORMAT_STRING "type=%s", INDENT_FORMAT_ARGS(2),
            vulkan_camera_type_debug_str(camera->type));
  if (camera->type == vulkan_camera_type_perspective) {
    log_debug(INDENT_FORMAT_STRING "fovY=%f", INDENT_FORMAT_ARGS(2), camera->fovY);
    log_debug(INDENT_FORMAT_STRING "aspectRatio=%f", INDENT_FORMAT_ARGS(2), camera->aspectRatio);
  } else if (camera->type == vulkan_camera_type_orthographic) {
    log_debug(INDENT_FORMAT_STRING "magX=%f", INDENT_FORMAT_ARGS(2), camera->magX);
    log_debug(INDENT_FORMAT_STRING "magY=%f", INDENT_FORMAT_ARGS(2), camera->magY);
  }
  log_debug(INDENT_FORMAT_STRING "nearZ=%f", INDENT_FORMAT_ARGS(2), camera->nearZ);
  log_debug(INDENT_FORMAT_STRING "farZ=%f", INDENT_FORMAT_ARGS(2), camera->farZ);
}
