#include "camera.h"
#include "../data/db.h"

vulkan_camera *vulkan_camera_create() {
  vulkan_camera *camera = core_alloc(sizeof(vulkan_camera));
  glm_vec3((vec3){0.0f, 0.0f, 0.0f}, camera->position);
  glm_quat_identity(camera->rotation);
  camera->fovY = 90.0f;
  camera->aspectRatio = 1.0f;
  camera->nearZ = 0.1f;
  camera->farZ = 256.0f;
  // TODO: Camera config using Lua.
  camera->position[2] = -1.0f;
  camera->dirty = true;
  return camera;
}

void vulkan_camera_destroy(vulkan_camera *camera) { core_free(camera); }

void vulkan_camera_update_aspect_ratio(vulkan_camera *camera, float aspectRatio) {
  camera->aspectRatio = aspectRatio;
  camera->dirty = true;
}

data_blob vulkan_camera_serialize(vulkan_camera *camera) {
  data_blob blob = {0};
  blob.size = SERIALIZE_FIELD_SIZE(camera->position, camera->rotation, camera->fovY,
                                   camera->aspectRatio, camera->nearZ, camera->farZ);
  blob.memory = core_alloc(blob.size);
  uint8_t *ptr = blob.memory;
  serialize_vec3(ptr, camera->position), ptr += sizeof(camera->position);
  serialize_vec4(ptr, camera->rotation), ptr += sizeof(camera->rotation);
  serialize_float(ptr, camera->fovY), ptr += sizeof(camera->fovY);
  serialize_float(ptr, camera->aspectRatio), ptr += sizeof(camera->aspectRatio);
  serialize_float(ptr, camera->nearZ), ptr += sizeof(camera->nearZ);
  serialize_float(ptr, camera->farZ), ptr += sizeof(camera->farZ);
  return blob;
}

void vulkan_camera_deserialize(vulkan_camera *camera, data_blob blob) {
  uint8_t *ptr = blob.memory;
  deserialize_vec3(ptr, &camera->position), ptr += sizeof(camera->position);
  deserialize_vec4(ptr, &camera->rotation), ptr += sizeof(camera->rotation);
  deserialize_float(ptr, &camera->fovY), ptr += sizeof(camera->fovY);
  deserialize_float(ptr, &camera->aspectRatio), ptr += sizeof(camera->aspectRatio);
  deserialize_float(ptr, &camera->nearZ), ptr += sizeof(camera->nearZ);
  deserialize_float(ptr, &camera->farZ), ptr += sizeof(camera->farZ);
}
