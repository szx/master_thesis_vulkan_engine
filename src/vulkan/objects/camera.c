#include "camera.h"
#include "../scene/node.h"

vulkan_camera *vulkan_camera_create(vulkan_render_cache_list *renderCacheList) {
  vulkan_camera *camera = core_alloc(sizeof(vulkan_camera));

  camera->renderCacheList = renderCacheList;

  // HIRO HIRO orthographic cameras + selection via camera idx (camera 1 preferred)
  camera->cameraIdx = 0;
  assert(utarray_len(camera->renderCacheList->cameraRenderCaches) > 0);
  camera->renderCache =
      *(vulkan_render_cache **)utarray_eltptr(camera->renderCacheList->cameraRenderCaches, 0);

  return camera;
}

void vulkan_camera_destroy(vulkan_camera *camera) { core_free(camera); }

void vulkan_camera_set_view_matrix(vulkan_camera *camera, mat4 viewMatrix) {
  // View matrix is inversed model matrix.
  glm_mat4_inv(camera->renderCache->transform, viewMatrix);
}

/// Calculates perspective matrix for left handed world/view-space to left-handed flipped Vulkan
/// clip space.
void get_perspective_matrix(float fovy, float aspect, float nearZ, float farZ, mat4 dest) {
  glm_mat4_zero(dest);

  // https://vincent-p.github.io/posts/vulkan_perspective_matrix/
  float f;
  f = 1.0f / tanf(fovy * 0.5f);
  float x = f / aspect;
  float y = -f;
  float A = nearZ / (farZ - nearZ);
  float B = (nearZ * farZ) / (farZ - nearZ);

  dest[0][0] = x;
  dest[1][1] = y;
  dest[2][2] = A;
  dest[2][3] = -1.0f;
  dest[3][2] = B;
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
