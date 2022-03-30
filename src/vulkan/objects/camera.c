#include "camera.h"
#include "../scene/node.h"

vulkan_camera *vulkan_camera_create(vulkan_render_cache_list *renderCacheList) {
  vulkan_camera *camera = core_alloc(sizeof(vulkan_camera));

  camera->renderCacheList = renderCacheList;

  camera->cameraIdx = 0;
  vulkan_camera_select(camera, camera->cameraIdx);

  return camera;
}

void vulkan_camera_destroy(vulkan_camera *camera) { core_free(camera); }

void vulkan_camera_select(vulkan_camera *camera, size_t cameraIdx) {
  camera->cameraIdx = cameraIdx % utarray_len(camera->renderCacheList->cameraRenderCaches);
  log_debug("selecting camera %zu", camera->cameraIdx);

  assert(utarray_len(camera->renderCacheList->cameraRenderCaches) > 0);
  assert(camera->cameraIdx < utarray_len(camera->renderCacheList->cameraRenderCaches));
  camera->renderCache = *(vulkan_render_cache **)utarray_eltptr(
      camera->renderCacheList->cameraRenderCaches, camera->cameraIdx);
}

void vulkan_camera_set_view_matrix(vulkan_camera *camera, mat4 viewMatrix) {
  // View matrix is inversed model matrix.
  glm_mat4_inv(camera->renderCache->transform, viewMatrix);
}

/// Calculates perspective projection matrix for left handed world/view-space to right-handed
/// flipped Vulkan clip space.
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

/// Calculates orthographic projection matrix for left handed world/view-space to right-handed
/// flipped Vulkan clip space.
void get_orthographic_matrix(float r, float t, float nearZ, float farZ, mat4 dest) {
  glm_mat4_zero(dest);

  // https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#orthographic-projection
  // https://dev.theomader.com/depth-precision/
  dest[0][0] = r;
  dest[1][1] = -t;
  dest[2][2] = 2.0f / (nearZ - farZ);
  dest[3][2] = (farZ + nearZ) / (nearZ - farZ);
  dest[3][3] = 1.0f;

  // Reverse Z:
  dest[2][2] = -dest[2][2];
  dest[3][2] = -dest[3][2];
}

void vulkan_camera_set_projection_matrix(vulkan_camera *camera, mat4 projectionMatrix) {
  vulkan_data_camera *cameraData = &camera->renderCache->camera;
  if (cameraData->type == vulkan_camera_type_perspective) {
    get_perspective_matrix(cameraData->fovY, cameraData->aspectRatio, cameraData->nearZ,
                           cameraData->farZ, projectionMatrix);
  } else if (cameraData->type == vulkan_camera_type_orthographic) {
    get_orthographic_matrix(cameraData->magX, cameraData->magY, cameraData->nearZ, cameraData->farZ,
                            projectionMatrix);
  }
}

void vulkan_camera_debug_print(vulkan_camera *camera, int indent) {
  log_debug(INDENT_FORMAT_STRING "camera %zu:", INDENT_FORMAT_ARGS(0), camera->cameraIdx);
  vulkan_render_cache_debug_print(camera->renderCache);
}
