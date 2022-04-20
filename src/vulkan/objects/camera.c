#include "camera.h"
#include "../scene/node.h"

vulkan_camera *vulkan_camera_create(vulkan_render_cache_list *renderCacheList,
                                    vulkan_swap_chain *vks) {
  vulkan_camera *camera = core_alloc(sizeof(vulkan_camera));

  camera->renderCacheList = renderCacheList;
  camera->vks = vks;

  camera->defaultCameraRenderCache = vulkan_render_cache_create(NULL);
  vulkan_render_cache_list_add_camera_render_cache(camera->renderCacheList,
                                                   camera->defaultCameraRenderCache);
  vulkan_camera_reset(camera);

  return camera;
}

void vulkan_camera_destroy(vulkan_camera *camera) {
  vulkan_render_cache_destroy(camera->defaultCameraRenderCache);
  core_free(camera);
}

void vulkan_camera_reset(vulkan_camera *camera) {
  // Set up default camera using primitives' aabb.
  vulkan_aabb *aabb = &camera->renderCacheList->aabb;
  vec3 extent, center;
  glm_vec3_sub(aabb->max, aabb->min, extent);
  glm_vec3_add(aabb->max, aabb->min, center);
  glm_vec3_divs(center, 2.0f, center);

  if (extent[2] > 0) {
    // Try to fix camera clipping.
    float defaultNearZ = extent[2] / 10;
    camera->defaultCameraRenderCache->camera.nearZ =
        MIN(defaultNearZ, camera->defaultCameraRenderCache->camera.nearZ);
  }

  vec3 extentAbs;
  glm_vec3_abs(extent, extentAbs);
  camera->speed = glm_vec3_min(extentAbs);

  // NOTE: Default camera looks at 0,0,0 from 0,0,-max(extent).
  // TODO: Default camera look at center.
  float distanceZ = glm_vec3_max(extentAbs) * -1.0f;
  glm_translate_z(camera->defaultCameraRenderCache->transform, distanceZ);

  // NOTE: Change right-handed model-space into left-handed world-space (camera render cache are
  // already flipped thanks to root transform).
  camera->defaultCameraRenderCache->transform[2][2] = -1.0f;

  camera->cameraIdx = 0;
  vulkan_camera_select(camera, camera->cameraIdx);
}

void vulkan_camera_select(vulkan_camera *camera, size_t cameraIdx) {
  assert(utarray_len(camera->renderCacheList->cameraRenderCaches) > 0);

  camera->cameraIdx = cameraIdx % utarray_len(camera->renderCacheList->cameraRenderCaches);
  log_debug("selecting camera %zu", camera->cameraIdx);

  assert(utarray_len(camera->renderCacheList->cameraRenderCaches) > 0);
  assert(camera->cameraIdx < utarray_len(camera->renderCacheList->cameraRenderCaches));
  camera->cameraRenderCache = *(vulkan_render_cache **)utarray_eltptr(
      camera->renderCacheList->cameraRenderCaches, camera->cameraIdx);

  glm_mat4_identity(camera->userTransform);
}

void vulkan_camera_set_view_matrix(vulkan_camera *camera, mat4 viewMatrix) {
  // View matrix is inversed model matrix.
  mat4 transform;
  glm_mat4_mul(camera->userTransform, camera->cameraRenderCache->transform, transform);
  glm_mat4_inv(transform, viewMatrix);
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
  vulkan_data_camera *cameraData = &camera->cameraRenderCache->camera;
  if (cameraData->type == vulkan_camera_type_perspective) {
    float viewportAspectRatio = vulkan_swap_chain_get_aspect_ratio(camera->vks);
    get_perspective_matrix(cameraData->fovY, viewportAspectRatio, cameraData->nearZ,
                           cameraData->farZ, projectionMatrix);
  } else if (cameraData->type == vulkan_camera_type_orthographic) {
    // TODO: Adjust magX and magY using vks.
    get_orthographic_matrix(cameraData->magX, cameraData->magY, cameraData->nearZ, cameraData->farZ,
                            projectionMatrix);
  }
}

void vulkan_camera_set_position(vulkan_camera *camera, vec3 position) {
  mat4 viewMat;
  vulkan_camera_set_view_matrix(camera, viewMat);
  mat4 viewMatInv;
  glm_mat4_inv(viewMat, viewMatInv);
  glm_vec3_copy(viewMatInv[3], position);
}

void vulkan_camera_debug_print(vulkan_camera *camera, int indent) {
  log_debug(INDENT_FORMAT_STRING "camera %zu:", INDENT_FORMAT_ARGS(0), camera->cameraIdx);
  vulkan_render_cache_debug_print(camera->cameraRenderCache);
}
