/* Vulkan scene render.
 */
#pragma once

#include "../scene/graph.h"
#include "render_graph.h"
#include "render_state.h"

/// Creates and destroys Vulkan objects used to draw scene described by scene graph.
typedef struct renderer {
  /* CPU state */
  data_config *config; ///< Pointer.
  asset_db *assetDb;   ///< Pointer.
  scene_data *data;
  renderer_cache *rendererCache;
  scene_graph *sceneGraph;

  /* GPU state */
  device *vkd;     ///< Pointer.
  swap_chain *vks; ///< Pointer.
  render_state *renderState;
  render_graph *renderGraph;

} renderer;

renderer *renderer_create(data_config *config, asset_db *assetDb, swap_chain *vks,
                          UT_string *sceneName);
void renderer_destroy(renderer *renderer);

void renderer_recreate_swap_chain(renderer *renderer);

void renderer_update(renderer *renderer);

void renderer_update_unified_constant_buffer_callback(void *data, size_t currentFrameInFlight,
                                                      global_uniform_buffer_data *globalData,
                                                      instances_uniform_buffer_data *instancesData);

void renderer_send_to_device(renderer *renderer);

void renderer_draw_frame(renderer *renderer);

typedef void (*renderer_main_loop_update_func)(renderer *renderer, double fps, double dt);

void renderer_run_main_loop(renderer *renderer, renderer_main_loop_update_func updateFunc);

void renderer_exit_main_loop(renderer *renderer);

void renderer_debug_print(renderer *renderer);
