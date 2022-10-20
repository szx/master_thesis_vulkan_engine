/* Vulkan scene render.
 */
#pragma once

#include "../scene/graph.h"
#include "render_graph.h"
#include "render_state.h"

/// Creates and destroys Vulkan objects used to draw scene described by scene graph.
typedef struct renderer {
  /* CPU state */
  data_config *config;
  asset_db *assetDb;

  scene_data *data;
  renderer_cache *rendererCache;
  scene_graph *sceneGraph;

  /* GPU state */
  device *vkd;
  swap_chain *vks;

  render_state *renderState;
  render_graph *renderGraph;

} renderer;

renderer *renderer_create();
void renderer_destroy(renderer *renderer);

void renderer_recreate_swap_chain(renderer *renderer);

void renderer_update(renderer *renderer);

void renderer_send_to_device(renderer *renderer);

void renderer_draw_frame(renderer *renderer);

typedef void (*renderer_main_loop_update_func)(renderer *renderer, double fps, double dt);

void renderer_run_main_loop(renderer *renderer, renderer_main_loop_update_func updateFunc);

void renderer_exit_main_loop(renderer *renderer);

void renderer_debug_print(renderer *renderer);
