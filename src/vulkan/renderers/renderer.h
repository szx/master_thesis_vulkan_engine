/* Vulkan scene render.
 */
#pragma once

#include "../scene/graph.h"
#include "render_graph.h"
#include "render_state.h"

/// Creates and destroys Vulkan objects used to draw scene described by scene graph.
typedef struct vulkan_renderer {
  /* CPU state */
  data_config *config;    ///< Pointer.
  data_asset_db *assetDb; ///< Pointer.
  vulkan_scene_data *data;
  vulkan_renderer_cache *rendererCache;
  vulkan_scene_graph *sceneGraph;

  /* GPU state */
  vulkan_device *vkd;     ///< Pointer.
  vulkan_swap_chain *vks; ///< Pointer.
  vulkan_render_state *renderState;
  vulkan_render_graph *renderGraph;

} vulkan_renderer;

vulkan_renderer *vulkan_renderer_create(data_config *config, data_asset_db *assetDb,
                                        vulkan_swap_chain *vks, UT_string *sceneName,
                                        vulkan_render_pass_type *renderPasss,
                                        size_t renderPassCount);
void vulkan_renderer_destroy(vulkan_renderer *renderer);

void vulkan_renderer_recreate_swap_chain(vulkan_renderer *renderer);

void vulkan_renderer_update(vulkan_renderer *renderer);

void vulkan_renderer_update_unified_uniform_buffer_callback(
    void *data, size_t currentFrameInFlight, vulkan_global_uniform_buffer_data *globalData,
    vulkan_instances_uniform_buffer_data *instancesData);

void vulkan_renderer_send_to_device(vulkan_renderer *renderer);

void vulkan_renderer_draw_frame(vulkan_renderer *renderer);

typedef void (*vulkan_renderer_main_loop_update_func)(vulkan_renderer *renderer, double fps,
                                                      double dt);

void vulkan_renderer_run_main_loop(vulkan_renderer *renderer,
                                   vulkan_renderer_main_loop_update_func updateFunc);

void vulkan_renderer_exit_main_loop(vulkan_renderer *renderer);

void vulkan_renderer_debug_print(vulkan_renderer *renderer);
