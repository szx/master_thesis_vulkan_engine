#include "camera_state.h"

vulkan_render_pass_camera_state *
vulkan_render_pass_camera_state_create(vulkan_render_state *renderState) {
  vulkan_render_pass_camera_state *camera = core_alloc(sizeof(vulkan_render_pass_camera_state));

  camera->renderState = renderState;

  camera->cameraIdx = 0;
  vulkan_render_pass_camera_state_select(camera, camera->cameraIdx);

  return camera;
}

void vulkan_render_pass_camera_state_destroy(vulkan_render_pass_camera_state *camera) {
  core_free(camera);
}

void vulkan_render_pass_camera_state_reinit_with_new_swap_chain(
    vulkan_render_pass_camera_state *camera) {
  // No-op.
}

void vulkan_render_pass_camera_state_update(vulkan_render_pass_camera_state *camera) {
  glm_mat4_identity(camera->user.transform);

  mat4 translation;
  glm_translate_make(translation, camera->user.position);

  mat4 rotation;
  glm_euler_yxz((vec3){camera->user.pitch, camera->user.yaw, camera->user.roll}, rotation);

  glm_mat4_mul(translation, rotation, camera->user.transform);
}

void vulkan_render_pass_camera_state_select(vulkan_render_pass_camera_state *camera,
                                            size_t cameraIdx) {

  size_t cameraCount = 1 + dl_count(camera->renderState->rendererCache->cameraElements);
  camera->cameraIdx = cameraIdx % (cameraCount);

  if (camera->cameraIdx == 0) {
    log_debug("selecting default camera %zu", camera->cameraIdx);
    camera->cameraElement = camera->renderState->rendererCache->defaultCameraElement;
  } else {
    log_debug("selecting camera %zu", camera->cameraIdx);
    camera->cameraElement =
        dl_elt(camera->renderState->rendererCache->cameraElements, camera->cameraIdx - 1);
  }
  vulkan_render_pass_camera_state_reset(camera);
}

void reset_default_camera(vulkan_render_pass_camera_state *camera, vec3 distance) {
  glm_mat4_identity(camera->renderState->rendererCache->defaultCameraElement->transform);
  glm_translate(camera->renderState->rendererCache->defaultCameraElement->transform, distance);
  // NOTE: Change right-handed model-space into left-handed world-space (renderer cache camera
  // element are already flipped thanks to root transform).
  camera->renderState->rendererCache->defaultCameraElement->transform[2][2] = -1.0f;
}

void update_camera_vectors(vulkan_render_pass_camera_state *camera) {
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

void vulkan_render_pass_camera_state_reset(vulkan_render_pass_camera_state *camera) {
  // Set up default camera using primitives' aabb.
  vulkan_renderer_cache_calculate_aabb_for_primitive_elements(camera->renderState->rendererCache);
  vulkan_aabb *aabb = &camera->renderState->rendererCache->aabb;

  vec3 extent, center;
  glm_vec3_sub(aabb->max, aabb->min, extent);
  glm_vec3_add(aabb->max, aabb->min, center);
  glm_vec3_divs(center, 2.0f, center);

  if (extent[2] > 0) {
    // Try to fix default camera clipping.
    float defaultNearZ = extent[2] / 10;
    camera->renderState->rendererCache->defaultCameraElement->camera->nearZ =
        MIN(defaultNearZ, camera->renderState->rendererCache->defaultCameraElement->camera->nearZ);
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
  glm_vec3_copy(WORLD_UP, camera->user.up);
  camera->user.yaw = 0;
  camera->user.pitch = 0;
  camera->user.roll = 0;

  reset_default_camera(camera, GLM_VEC3_ZERO);

  update_camera_vectors(camera);
  vulkan_render_pass_camera_state_update(camera);
}

void vulkan_render_pass_camera_state_move(vulkan_render_pass_camera_state *camera, float frontDt,
                                          float rightDt, float upDt) {
  glm_vec3_muladds(camera->user.front, frontDt, camera->user.position);
  glm_vec3_muladds(camera->user.right, rightDt, camera->user.position);
  glm_vec3_muladds(camera->user.up, upDt, camera->user.position);
}

void vulkan_render_pass_camera_state_rotate(vulkan_render_pass_camera_state *camera, float yawDt,
                                            float pitchDt, float rollDt) {
  camera->user.yaw += yawDt;
  camera->user.pitch += pitchDt;
  camera->user.roll += rollDt;
  update_camera_vectors(camera);
}

void vulkan_render_pass_camera_state_set_view_matrix(vulkan_render_pass_camera_state *camera,
                                                     mat4 viewMatrix) {
  // View matrix is inversed model matrix.
  mat4 transform;
  if (camera->cameraElement == camera->renderState->rendererCache->defaultCameraElement) {
    glm_mat4_mul(camera->user.transform, camera->cameraElement->transform, transform);
  } else {
    glm_mat4_copy(camera->cameraElement->transform, transform);
  }
  glm_mat4_inv(transform, viewMatrix);
}

void vulkan_render_pass_camera_state_set_projection_matrix(vulkan_render_pass_camera_state *camera,
                                                           mat4 projectionMatrix) {
  vulkan_asset_camera *cameraData = camera->cameraElement->camera;
  if (cameraData->type == vulkan_camera_type_perspective) {
    float viewportAspectRatio = vulkan_swap_chain_get_aspect_ratio(camera->renderState->vks);
    vulkan_get_perspective_matrix(cameraData->fovY, viewportAspectRatio, cameraData->nearZ,
                                  cameraData->farZ, projectionMatrix);
  } else if (cameraData->type == vulkan_camera_type_orthographic) {
    // TODO: Adjust magX and magY using vks.
    vulkan_get_orthographic_matrix(-cameraData->magX, cameraData->magX, -cameraData->magY,
                                   cameraData->magY, cameraData->nearZ, cameraData->farZ,
                                   projectionMatrix);
  }
}

void vulkan_render_pass_camera_state_set_position(vulkan_render_pass_camera_state *camera,
                                                  vec3 position) {
  mat4 viewMat;
  vulkan_render_pass_camera_state_set_view_matrix(camera, viewMat);
  mat4 viewMatInv;
  glm_mat4_inv(viewMat, viewMatInv);
  glm_vec3_copy(viewMatInv[3], position);
}

void vulkan_render_pass_camera_state_debug_print(vulkan_render_pass_camera_state *camera,
                                                 int indent) {
  log_debug(INDENT_FORMAT_STRING "camera %zu:", INDENT_FORMAT_ARGS(0), camera->cameraIdx);
  vulkan_renderer_cache_camera_element_debug_print(camera->cameraElement);
}
