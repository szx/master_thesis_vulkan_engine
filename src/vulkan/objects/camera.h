/* Vulkan camera.
 */
#pragma once

#include "../common.h"
#include "render_cache_list.h"
#include "swap_chain.h"

typedef struct vulkan_camera {
  vulkan_render_cache_list *renderCacheList; ///< Pointer.
  vulkan_swap_chain *vks;                    ///< Pointer.

  size_t cameraIdx;
  vulkan_render_cache *cameraRenderCache; ///< Pointer.
  vulkan_render_cache *defaultCameraRenderCache;

  mat4 userTransform;
  float speed; ///< Used to make sure that userTransform is not changed too quickly in small scenes.

} vulkan_camera;

vulkan_camera *vulkan_camera_create(vulkan_render_cache_list *renderCacheList,
                                    vulkan_swap_chain *vks);
void vulkan_camera_destroy(vulkan_camera *camera);

void vulkan_camera_reset(vulkan_camera *camera);

void vulkan_camera_select(vulkan_camera *camera, size_t cameraIdx);

void vulkan_camera_set_view_matrix(vulkan_camera *camera, mat4 viewMatrix);
void vulkan_camera_set_projection_matrix(vulkan_camera *camera, mat4 projectionMatrix);

void vulkan_camera_debug_print(vulkan_camera *camera, int indent);
