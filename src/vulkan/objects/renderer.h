/* Vulkan scene render.
 */
#pragma once

#include "../scene/graph.h"
#include "pipeline.h"
#include "render_state.h"

/// Creates and destroys Vulkan objects used to draw scene described by scene graph.
typedef struct vulkan_renderer {
  /* CPU state */
  data_config *config;    ///< Pointer.
  data_asset_db *assetDb; ///< Pointer.
  vulkan_data_scene *data;
  vulkan_render_cache_list *renderCacheList;
  vulkan_scene_graph *sceneGraph;

  /* GPU state */
  vulkan_device *vkd;     ///< Pointer.
  vulkan_swap_chain *vks; ///< Pointer.
  vulkan_render_state *renderState;
  vulkan_pipeline *pipeline;
} vulkan_renderer;

vulkan_renderer *vulkan_renderer_create(data_config *config, data_asset_db *assetDb,
                                        vulkan_swap_chain *vks, UT_string *sceneName);
void vulkan_renderer_destroy(vulkan_renderer *renderer);

void vulkan_renderer_update(vulkan_renderer *renderer);

void vulkan_renderer_send_to_device(vulkan_renderer *renderer);

void vulkan_renderer_draw_frame(vulkan_renderer *renderer);

void vulkan_renderer_debug_print(vulkan_renderer *renderer);
