/* Vulkan camera.
 */
#pragma once

#include "../../render_state.h"

// TODO: Frustum plane CPU culling.

typedef struct render_pass_camera_state {
  render_state *renderState; ///< Pointer.

  size_t cameraIdx;
  renderer_cache_camera_element *cameraElement; ///< Pointer.

  struct {
    float moveSpeed;
    float rotationSpeed;
    vec3 position;
    vec3 front, up, right;
    float yaw;      ///< Y-axis Euler angle (in radians).
    float pitch;    ///< X-axis Euler angle (in radians).
    float roll;     ///< Z-axis Euler angle (in radians).
    mat4 transform; ///< Final transform after translation and yaw, pitch and roll rotations.
  } user;
} render_pass_camera_state;

render_pass_camera_state *render_pass_camera_state_create(render_state *renderState);
void render_pass_camera_state_destroy(render_pass_camera_state *camera);
void render_pass_camera_state_reinit_with_new_swap_chain(render_pass_camera_state *camera);
void render_pass_camera_state_update(render_pass_camera_state *camera);

void render_pass_camera_state_select(render_pass_camera_state *camera, size_t cameraIdx);
void render_pass_camera_state_reset(render_pass_camera_state *camera);
void render_pass_camera_state_move(render_pass_camera_state *camera, float frontDt, float rightDt,
                                   float upDt);
void render_pass_camera_state_rotate(render_pass_camera_state *camera, float yawDt, float pitchDt,
                                     float rollDt);

void render_pass_camera_state_set_view_matrix(render_pass_camera_state *camera, mat4 viewMatrix);
void render_pass_camera_state_set_projection_matrix(render_pass_camera_state *camera,
                                                    mat4 projectionMatrix);
void render_pass_camera_state_set_position(render_pass_camera_state *camera, vec3 position);

void render_pass_camera_state_debug_print(render_pass_camera_state *camera, int indent);
