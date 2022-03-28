/* Vulkan camera.
 */
#pragma once

#include "../common.h"
#include "render_cache_list.h"

typedef struct vulkan_camera {
  vulkan_render_cache_list *renderCacheList; ///< Pointer.

  size_t cameraIdx;
  vulkan_render_cache *renderCache; ///< Pointer.

} vulkan_camera;

vulkan_camera *vulkan_camera_create(vulkan_render_cache_list *renderCacheList);
void vulkan_camera_destroy(vulkan_camera *camera);

void vulkan_camera_set_view_matrix(vulkan_camera *camera, mat4 viewMatrix);
void vulkan_camera_set_projection_matrix(vulkan_camera *camera, mat4 projectionMatrix);

void vulkan_camera_debug_print(vulkan_camera *camera, int indent);
