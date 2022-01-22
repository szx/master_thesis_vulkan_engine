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
  data_blob blob;
  blob.size = 11 * sizeof(float);
  blob.memory = core_alloc(blob.size);
  float *ptr = blob.memory;
#define SERIALIZE_FLOAT(_ptr, _field)                                                              \
  do {                                                                                             \
    *_ptr = camera->_field;                                                                        \
    _ptr++;                                                                                        \
  } while (0)
  SERIALIZE_FLOAT(ptr, position[0]);
  SERIALIZE_FLOAT(ptr, position[1]);
  SERIALIZE_FLOAT(ptr, position[2]);
  SERIALIZE_FLOAT(ptr, rotation[0]);
  SERIALIZE_FLOAT(ptr, rotation[1]);
  SERIALIZE_FLOAT(ptr, rotation[2]);
  SERIALIZE_FLOAT(ptr, rotation[3]);
  SERIALIZE_FLOAT(ptr, fovY);
  SERIALIZE_FLOAT(ptr, aspectRatio);
  SERIALIZE_FLOAT(ptr, nearZ);
  SERIALIZE_FLOAT(ptr, farZ);
#undef SERIALIZE_FLOAT
  return blob;
}

void vulkan_camera_update_uniform_buffer_data(vulkan_camera *camera,
                                              vulkan_uniform_buffer *uniformBuffer) {
  // HIRO move vulkan_camera_update_uniform_buffer_data to build in scene - only data here.
  if (!camera->dirty) {
    return;
  }
  vec3 negativePosition;
  mat4 translationMat;
  glm_vec3_negate_to(camera->position, negativePosition);
  glm_translate_make(translationMat, negativePosition);
  mat4 rotationMat;
  glm_quat_mat4(camera->rotation, rotationMat);
  glm_mat4_mul(rotationMat, translationMat, uniformBuffer->data.viewMat);

  glm_perspective(camera->fovY, camera->aspectRatio, camera->nearZ, camera->farZ,
                  uniformBuffer->data.projMat);
  uniformBuffer->data.projMat[1][1] *= -1; // invert for Y+ pointing up
  camera->dirty = false;
  uniformBuffer->dirty = true;
}
