/* Vulkan scene render.
 */
#pragma once

#include "../objects/pipeline.h"
#include "../objects/render_state.h"
#include "graph.h"

/// Creates and destroys Vulkan objects used to draw scene described by scene data entities.
typedef struct vulkan_scene_renderer {
  /* CPU state */
  data_asset_db *assetDb; ///< Pointer.
  vulkan_data_scene *data;
  vulkan_render_cache_list *renderCacheList;
  vulkan_scene_graph *sceneGraph;
  vulkan_data_camera camera;

  /* GPU state */
  vulkan_device *vkd;     ///< Pointer.
  vulkan_swap_chain *vks; ///< Pointer.
  vulkan_render_state *renderState;
  vulkan_pipeline *pipeline;

} vulkan_scene_renderer;

vulkan_scene_renderer *vulkan_scene_renderer_create(data_asset_db *assetDb, vulkan_swap_chain *vks,
                                                    UT_string *sceneName);
void vulkan_scene_renderer_destroy(vulkan_scene_renderer *renderer);

void vulkan_scene_renderer_update(vulkan_scene_renderer *renderer);
void vulkan_scene_renderer_send_to_device(vulkan_scene_renderer *renderer);

void vulkan_scene_renderer_debug_print(vulkan_scene_renderer *renderer);