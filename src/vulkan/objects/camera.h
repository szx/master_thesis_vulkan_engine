/* Vulkan camera.
 */
#pragma once

#include "../common.h"
#include "render_cache_list.h"
#include "swap_chain.h"

// TODO: Frustum planes.

typedef struct vulkan_camera {
  vulkan_render_cache_list *renderCacheList; ///< Pointer.
  vulkan_swap_chain *vks;                    ///< Pointer.

  size_t cameraIdx;
  vulkan_render_cache *cameraRenderCache; ///< Pointer.
  vulkan_render_cache *defaultCameraRenderCache;

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
} vulkan_camera;

vulkan_camera *vulkan_camera_create(vulkan_render_cache_list *renderCacheList,
                                    vulkan_swap_chain *vks);
void vulkan_camera_destroy(vulkan_camera *camera);

void vulkan_camera_reset(vulkan_camera *camera);

void vulkan_camera_move(vulkan_camera *camera, float frontDt, float rightDt, float upDt);
void vulkan_camera_rotate(vulkan_camera *camera, float yawDt, float pitchDt, float rollDt);
void vulkan_camera_update(vulkan_camera *camera);

void vulkan_camera_select(vulkan_camera *camera, size_t cameraIdx);

void vulkan_camera_set_view_matrix(vulkan_camera *camera, mat4 viewMatrix);
void vulkan_camera_set_projection_matrix(vulkan_camera *camera, mat4 projectionMatrix);
void vulkan_camera_set_position(vulkan_camera *camera, vec3 position);

void vulkan_camera_debug_print(vulkan_camera *camera, int indent);
