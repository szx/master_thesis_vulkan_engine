#include "camera_state.h"

vulkan_camera_state *vulkan_camera_state_create(vulkan_render_state *renderState) {
  vulkan_camera_state *camera = core_alloc(sizeof(vulkan_camera_state));

  camera->renderState = renderState;

  camera->defaultCameraRenderCache = vulkan_render_cache_create(NULL);
  vulkan_render_cache_list_add_camera_render_cache(camera->renderState->renderCacheList,
                                                   camera->defaultCameraRenderCache);

  camera->cameraIdx = 0;
  vulkan_camera_state_select(camera, camera->cameraIdx);

  return camera;
}

void vulkan_camera_state_destroy(vulkan_camera_state *camera) {
  vulkan_render_cache_destroy(camera->defaultCameraRenderCache);
  core_free(camera);
}

void reset_default_camera(vulkan_camera_state *camera, vec3 distance) {
  glm_mat4_identity(camera->defaultCameraRenderCache->transform);
  glm_translate(camera->defaultCameraRenderCache->transform, distance);
  // NOTE: Change user.right-handed model-space into left-handed world-space (camera render cache
  // are already flipped thanks to root transform).
  camera->defaultCameraRenderCache->transform[2][2] = -1.0f;
}

void update_camera_vectors(vulkan_camera_state *camera) {
  // limit pitch to prevent disorienting sudden front vector flipping
  if (camera->user.pitch >= glm_rad(90.0f)) {
    camera->user.pitch = nextafterf(glm_rad(90.0f), -FLT_MAX);
  }
  if (camera->user.pitch <= glm_rad(-90.0f)) {
    camera->user.pitch = nextafterf(glm_rad(-90.0f), FLT_MAX);
  }
  camera->user.yaw = fmodf(camera->user.yaw, glm_rad(360.0f));

  // extract axes from rotation matrix
  mat4 rotation;
  glm_euler_yxz((vec3){camera->user.pitch, camera->user.yaw, camera->user.roll}, rotation);
  glm_vec3_copy(rotation[0], camera->user.right); // X-axis
  glm_vec3_copy(rotation[1], camera->user.up);    // Y-axis
  glm_vec3_copy(rotation[2], camera->user.front); // Z-axis

  glm_normalize(camera->user.front);
  glm_normalize(camera->user.right);
  glm_normalize(camera->user.up);
}

void vulkan_camera_state_reset(vulkan_camera_state *camera) {
  // Set up default camera using primitives' aabb.
  vulkan_render_cache_list_calculate_aabb_for_primitive_render_caches(
      camera->renderState->renderCacheList);
  vulkan_aabb *aabb = &camera->renderState->renderCacheList->aabb;

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
  camera->user.moveSpeed = 1.0f;
  if (extentAbs[0] > 0) {
    camera->user.moveSpeed = fminf(extentAbs[0], camera->user.moveSpeed);
  }
  if (extentAbs[1] > 0) {
    camera->user.moveSpeed = fminf(extentAbs[1], camera->user.moveSpeed);
  }
  if (extentAbs[2] > 0) {
    camera->user.moveSpeed = fminf(extentAbs[2], camera->user.moveSpeed);
  }
  camera->user.moveSpeed *= 2.0f;

  camera->user.rotationSpeed = 1.0f;
  // NOTE: Default camera looks at +X from center.x,center.y,-max(extent).
  vec3 distance = {center[0], center[1], -glm_vec3_max(extentAbs)};
  glm_vec3_copy(distance, camera->user.position);
  glm_vec3_copy((vec3){0, 0, 1}, camera->user.front);
  glm_vec3_copy((vec3){0, 1, 0}, camera->user.up);
  camera->user.yaw = 0;
  camera->user.pitch = 0;
  camera->user.roll = 0;

  reset_default_camera(camera, GLM_VEC3_ZERO);

  update_camera_vectors(camera);
  vulkan_camera_state_update(camera);
}

void vulkan_camera_state_move(vulkan_camera_state *camera, float frontDt, float rightDt,
                              float upDt) {
  glm_vec3_muladds(camera->user.front, frontDt, camera->user.position);
  glm_vec3_muladds(camera->user.right, rightDt, camera->user.position);
  glm_vec3_muladds(camera->user.up, upDt, camera->user.position);
}

void vulkan_camera_state_rotate(vulkan_camera_state *camera, float yawDt, float pitchDt,
                                float rollDt) {
  camera->user.yaw += yawDt;
  camera->user.pitch += pitchDt;
  camera->user.roll += rollDt;
  update_camera_vectors(camera);
}

void vulkan_camera_state_update(vulkan_camera_state *camera) {
  glm_mat4_identity(camera->user.transform);

  mat4 translation;
  glm_translate_make(translation, camera->user.position);

  mat4 rotation;
  glm_euler_yxz((vec3){camera->user.pitch, camera->user.yaw, camera->user.roll}, rotation);

  glm_mat4_mul(translation, rotation, camera->user.transform);
}

void vulkan_camera_state_select(vulkan_camera_state *camera, size_t cameraIdx) {
  assert(utarray_len(camera->renderState->renderCacheList->cameraRenderCaches) > 0);

  camera->cameraIdx =
      cameraIdx % utarray_len(camera->renderState->renderCacheList->cameraRenderCaches);
  log_debug("selecting camera %zu", camera->cameraIdx);

  assert(utarray_len(camera->renderState->renderCacheList->cameraRenderCaches) > 0);
  assert(camera->cameraIdx < utarray_len(camera->renderState->renderCacheList->cameraRenderCaches));
  camera->cameraRenderCache = *(vulkan_render_cache **)utarray_eltptr(
      camera->renderState->renderCacheList->cameraRenderCaches, camera->cameraIdx);

  vulkan_camera_state_reset(camera);
}

void vulkan_camera_state_set_view_matrix(vulkan_camera_state *camera, mat4 viewMatrix) {
  // View matrix is inversed model matrix.
  mat4 transform;
  if (camera->cameraRenderCache == camera->defaultCameraRenderCache) {
    glm_mat4_mul(camera->user.transform, camera->cameraRenderCache->transform, transform);
  } else {
    glm_mat4_copy(camera->cameraRenderCache->transform, transform);
  }
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

void vulkan_camera_state_set_projection_matrix(vulkan_camera_state *camera, mat4 projectionMatrix) {
  vulkan_asset_camera *cameraData = &camera->cameraRenderCache->camera;
  if (cameraData->type == vulkan_camera_type_perspective) {
    float viewportAspectRatio = vulkan_swap_chain_get_aspect_ratio(camera->renderState->vks);
    get_perspective_matrix(cameraData->fovY, viewportAspectRatio, cameraData->nearZ,
                           cameraData->farZ, projectionMatrix);
  } else if (cameraData->type == vulkan_camera_type_orthographic) {
    // TODO: Adjust magX and magY using vks.
    get_orthographic_matrix(cameraData->magX, cameraData->magY, cameraData->nearZ, cameraData->farZ,
                            projectionMatrix);
  }
}

void vulkan_camera_state_set_position(vulkan_camera_state *camera, vec3 position) {
  mat4 viewMat;
  vulkan_camera_state_set_view_matrix(camera, viewMat);
  mat4 viewMatInv;
  glm_mat4_inv(viewMat, viewMatInv);
  glm_vec3_copy(viewMatInv[3], position);
}

void vulkan_camera_state_debug_print(vulkan_camera_state *camera, int indent) {
  log_debug(INDENT_FORMAT_STRING "camera %zu:", INDENT_FORMAT_ARGS(0), camera->cameraIdx);
  vulkan_render_cache_debug_print(camera->cameraRenderCache);
}
