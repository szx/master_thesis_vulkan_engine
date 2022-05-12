/* Vulkan camera.
 */
#pragma once

#include "../common.h"
#include "../renderers/render_state.h"

// TODO: Frustum planes.

typedef struct vulkan_pipeline_camera_state {
  vulkan_render_state *renderState; ///< Pointer.

  size_t cameraIdx;
  vulkan_renderer_cache_camera_element *cameraElement; ///< Pointer.
  vulkan_renderer_cache_camera_element *defaultCameraElement;

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
} vulkan_pipeline_camera_state;

vulkan_pipeline_camera_state *vulkan_pipeline_camera_state_create(vulkan_render_state *renderState);
void vulkan_pipeline_camera_state_destroy(vulkan_pipeline_camera_state *camera);
void vulkan_pipeline_camera_state_reinit_with_new_swap_chain(vulkan_pipeline_camera_state *camera);
void vulkan_pipeline_camera_state_update(vulkan_pipeline_camera_state *camera);

void vulkan_pipeline_camera_state_select(vulkan_pipeline_camera_state *camera, size_t cameraIdx);
void vulkan_pipeline_camera_state_reset(vulkan_pipeline_camera_state *camera);
void vulkan_pipeline_camera_state_move(vulkan_pipeline_camera_state *camera, float frontDt,
                                       float rightDt, float upDt);
void vulkan_pipeline_camera_state_rotate(vulkan_pipeline_camera_state *camera, float yawDt,
                                         float pitchDt, float rollDt);


void vulkan_pipeline_camera_state_set_view_matrix(vulkan_pipeline_camera_state *camera,
                                                  mat4 viewMatrix);
void vulkan_pipeline_camera_state_set_projection_matrix(vulkan_pipeline_camera_state *camera,
                                                        mat4 projectionMatrix);
void vulkan_pipeline_camera_state_set_position(vulkan_pipeline_camera_state *camera, vec3 position);

void vulkan_pipeline_camera_state_debug_print(vulkan_pipeline_camera_state *camera, int indent);
