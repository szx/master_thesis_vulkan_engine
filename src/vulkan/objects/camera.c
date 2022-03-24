#include "camera.h"
#include "../scene/node.h"

vulkan_camera *vulkan_camera_create(vulkan_render_cache_list *renderCacheList) {
  vulkan_camera *camera = core_alloc(sizeof(vulkan_camera));

  camera->renderCacheList = renderCacheList;

  camera->cameraIdx = 0;
  assert(utarray_len(camera->renderCacheList->caches) > 0);
  camera->renderCache = *(vulkan_render_cache **)utarray_front(camera->renderCacheList->caches);

  return camera;
}

void vulkan_camera_destroy(vulkan_camera *camera) { core_free(camera); }

void vulkan_camera_set_view_matrix(vulkan_camera *camera, mat4 viewMatrix) {

  /*
  vec3 negativePosition;
  glm_vec3_negate_to(camera->position, negativePosition);
  mat4 translationMat;
  glm_translate_make(translationMat, negativePosition);
  mat4 rotationMat;
  glm_quat_mat4(camera->rotation, rotationMat);
  glm_mat4_mul(rotationMat, translationMat, element->viewMat);
  */
  // vec3 position;
  // glm_vec3_copy(camera->sceneNode->, position);
  //  position[2] = -position[2];
  //  glm_quat_look(position, camera->rotation, element->viewMat);
  glm_mat4_inv(camera->renderCache->transform, viewMatrix);
}

/// Calculates perspective matrix for left handed world/view to space right-handed vulkan clip
/// space.
void get_perspective_matrix(float fovy, float aspect, float nearZ, float farZ, mat4 dest) {

  glm_mat4_zero(dest);
  /*
  // https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#finite-perspective-projection
  float f, fn;
  f = 1.0f / tanf(fovy * 0.5f);
  float x = f / aspect;
  float y = f;
  float A = (f + nearZ) / (nearZ - f);
  float B = (2 * f * nearZ) / (nearZ - f);

  dest[0][0] = x;
  dest[1][1] = y;
  dest[2][2] = A;
  dest[2][3] = B;
  dest[3][2] = -1.0f;
  */

  float f, fn;
  f = 1.0f / tanf(fovy * 0.5f);
  float x = f / aspect;
  float y = -f;
  float A = nearZ / (f - nearZ);
  float B = f * A;

  dest[0][0] = x;
  dest[1][1] = y;
  dest[2][2] = A;
  dest[2][3] = B;
  dest[3][2] = -1.0f;
}

void vulkan_camera_set_projection_matrix(vulkan_camera *camera, mat4 projectionMatrix) {
  vulkan_data_camera *cameraData = &camera->renderCache->camera;
  get_perspective_matrix(cameraData->fovY, cameraData->aspectRatio, cameraData->nearZ,
                         cameraData->farZ, projectionMatrix);
}

void vulkan_camera_debug_print(vulkan_camera *camera, int indent) {
  log_debug(INDENT_FORMAT_STRING "camera %zu:", INDENT_FORMAT_ARGS(0), camera->cameraIdx);
  vulkan_render_cache_debug_print(camera->renderCache);
}
