#include "shared_state.h"

void render_pass_shared_state_init(render_pass_shared_state *sharedState,
                                   render_state *renderState) {
  sharedState->renderState = renderState;

  sharedState->camera = render_pass_camera_state_create(sharedState->renderState);
  sharedState->materials = render_pass_material_state_create(sharedState->renderState);
  sharedState->lights = render_pass_light_state_create(sharedState->renderState);
  sharedState->skybox = render_pass_skybox_state_create(sharedState->renderState);
  sharedState->font = render_pass_font_state_create(sharedState->renderState);
  sharedState->rendererCacheBatches = batches_create(sharedState->renderState->vkd);
}

void render_pass_shared_state_deinit(render_pass_shared_state *sharedState) {
  batches_destroy(sharedState->rendererCacheBatches);
  render_pass_font_state_destroy(sharedState->font);
  render_pass_skybox_state_destroy(sharedState->skybox);
  render_pass_light_state_destroy(sharedState->lights);
  render_pass_material_state_destroy(sharedState->materials);
  render_pass_camera_state_destroy(sharedState->camera);
}

void render_pass_shared_state_reinit_with_new_swap_chain(render_pass_shared_state *sharedState) {

  render_pass_font_state_reinit_with_new_swap_chain(sharedState->font);
  render_pass_skybox_state_reinit_with_new_swap_chain(sharedState->skybox);
  render_pass_light_state_reinit_with_new_swap_chain(sharedState->lights);
  render_pass_material_state_reinit_with_new_swap_chain(sharedState->materials);
  render_pass_camera_state_reinit_with_new_swap_chain(sharedState->camera);
}

void render_pass_shared_state_update(render_pass_shared_state *sharedState) {
  log_debug("updating renderPass shared state");

  render_pass_camera_state_update(sharedState->camera);
  render_pass_material_state_update(sharedState->materials);
  render_pass_light_state_update(sharedState->lights);
  render_pass_skybox_state_update(sharedState->skybox);
  render_pass_font_state_update(sharedState->font);

  renderer_cache_add_new_primitive_elements_to_batches(
      sharedState->renderState->rendererCache, sharedState->rendererCacheBatches,
      renderer_cache_primitive_element_source_type_scene_tree);
  batch_instancing_policy instancingPolicy =
      (sharedState->renderState->config->global.graphicsEnabledInstancing
           ? batch_instancing_policy_matching_vertex_attributes
           : batch_instancing_policy_no_instancing);
  batches_update(sharedState->rendererCacheBatches, instancingPolicy);
}

void render_pass_shared_state_send_to_device(render_pass_shared_state *sharedState) {
  // No-op.
}

void render_pass_shared_state_set_unified_uniform_buffer(render_pass_shared_state *sharedState,
                                                         global_uniform_buffer_element *global) {

  render_pass_camera_state *camera = sharedState->camera;
  render_pass_camera_state_set_view_matrix(camera, global->viewMat);
  render_pass_camera_state_set_projection_matrix(camera, global->projMat);

  render_pass_material_state *materials = sharedState->materials;
  render_pass_material_state_set_material_elements(materials, &global->materialCount,
                                                   global->materials);

  render_pass_light_state *lights = sharedState->lights;
  render_pass_light_state_set_direct_light_elements(lights, &global->directionalLightCount,
                                                    global->directionalLights,
                                                    &global->pointLightCount, global->pointLights);

  render_pass_skybox_state *skybox = sharedState->skybox;
  render_pass_skybox_state_set_skybox_elements(skybox, &global->skybox);

  render_pass_font_state *font = sharedState->font;
  render_pass_font_state_set_font_elements(font, &global->font);

  global->viewport.width = sharedState->renderState->vks->swapChainExtent.width;
  global->viewport.height = sharedState->renderState->vks->swapChainExtent.height;
}

void render_pass_shared_state_debug_print(render_pass_shared_state *sharedState, int indent) {
  log_debug(INDENT_FORMAT_STRING "shared state:", INDENT_FORMAT_ARGS(0));
  batches_debug_print(sharedState->rendererCacheBatches);
  render_pass_camera_state_debug_print(sharedState->camera, indent + 2);
  render_pass_material_state_debug_print(sharedState->materials, indent + 2);
  render_pass_light_state_debug_print(sharedState->lights, indent + 2);
  render_pass_skybox_state_debug_print(sharedState->skybox, indent + 2);
  render_pass_font_state_debug_print(sharedState->font, indent + 2);
}
