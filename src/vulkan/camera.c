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

SERIALIZE_DEF(vulkan_camera, position[0], position[1], position[2], rotation[0], rotation[1],
              rotation[2], rotation[3], fovY, aspectRatio, nearZ, farZ)
DESERIALIZE_DEF(vulkan_camera, position[0], position[1], position[2], rotation[0], rotation[1],
                rotation[2], rotation[3], fovY, aspectRatio, nearZ, farZ)

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
