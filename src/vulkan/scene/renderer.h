/* Vulkan scene render.
 */
#pragma once

#include "../objects/batch.h"
#include "graph.h"

#include "../objects/unified_geometry_buffer.h"
#include "../objects/unified_uniform_buffer.h"

/// Creates and destroys Vulkan objects used to draw scene described by scene data entities.
typedef struct vulkan_scene_renderer {
  /* CPU state */
  data_asset_db *assetDb; /// Pointer.
  vulkan_data_scene *data;
  vulkan_render_cache_list *renderCacheList;
  vulkan_batches *batches;
  vulkan_scene_graph *sceneGraph;

  /* GPU state */
  vulkan_device *vkd; /// Pointer.
  vulkan_unified_geometry_buffer *unifiedGeometryBuffer;
  vulkan_unified_uniform_buffer *unifiedUniformBuffer;
} vulkan_scene_renderer;

vulkan_scene_renderer *vulkan_scene_renderer_create(data_asset_db *assetDb, vulkan_device *vkd,
                                                    UT_string *sceneName);
void vulkan_scene_renderer_destroy(vulkan_scene_renderer *renderer);

void vulkan_scene_renderer_build_geometry_buffer(
    vulkan_scene_renderer *renderer); // TODO move to separate header
/// Update scene data on CPU. Can make camera and uniform buffer dirty.
void vulkan_scene_renderer_update_data(vulkan_scene_renderer *renderer);
void vulkan_scene_renderer_send_to_device(vulkan_scene_renderer *renderer);

void vulkan_unified_uniform_buffer_update_with_camera(vulkan_unified_uniform_buffer *uniformBuffer,
                                                      vulkan_data_camera *camera);

void vulkan_scene_renderer_debug_print(vulkan_scene_renderer *renderer); // HIRO rewrite
