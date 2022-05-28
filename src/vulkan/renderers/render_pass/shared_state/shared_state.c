#include "shared_state.h"

void vulkan_render_pass_shared_state_init(vulkan_render_pass_shared_state *sharedState,
                                          vulkan_render_state *renderState) {
  sharedState->renderState = renderState;

  sharedState->camera = vulkan_render_pass_camera_state_create(sharedState->renderState);
  sharedState->materials = vulkan_render_pass_material_state_create(sharedState->renderState);
  sharedState->lights = vulkan_render_pass_light_state_create(sharedState->renderState);
  sharedState->skybox = vulkan_render_pass_skybox_state_create(sharedState->renderState);
  sharedState->font = vulkan_render_pass_font_state_create(sharedState->renderState);
  sharedState->rendererCacheBatches = vulkan_batches_create(sharedState->renderState->vkd);
}

void vulkan_render_pass_shared_state_deinit(vulkan_render_pass_shared_state *sharedState) {
  vulkan_batches_destroy(sharedState->rendererCacheBatches);
  vulkan_render_pass_font_state_destroy(sharedState->font);
  vulkan_render_pass_skybox_state_destroy(sharedState->skybox);
  vulkan_render_pass_light_state_destroy(sharedState->lights);
  vulkan_render_pass_material_state_destroy(sharedState->materials);
  vulkan_render_pass_camera_state_destroy(sharedState->camera);
}

void vulkan_render_pass_shared_state_reinit_with_new_swap_chain(
    vulkan_render_pass_shared_state *sharedState) {

  vulkan_render_pass_font_state_reinit_with_new_swap_chain(sharedState->font);
  vulkan_render_pass_skybox_state_reinit_with_new_swap_chain(sharedState->skybox);
  vulkan_render_pass_light_state_reinit_with_new_swap_chain(sharedState->lights);
  vulkan_render_pass_material_state_reinit_with_new_swap_chain(sharedState->materials);
  vulkan_render_pass_camera_state_reinit_with_new_swap_chain(sharedState->camera);
}

void vulkan_render_pass_shared_state_update(vulkan_render_pass_shared_state *sharedState) {
  log_debug("updating renderPass shared state");

  vulkan_render_pass_camera_state_update(sharedState->camera);
  vulkan_render_pass_material_state_update(sharedState->materials);
  vulkan_render_pass_light_state_update(sharedState->lights);
  vulkan_render_pass_skybox_state_update(sharedState->skybox);
  vulkan_render_pass_font_state_update(sharedState->font);

  vulkan_renderer_cache_add_new_primitive_elements_to_batches(
      sharedState->renderState->rendererCache, sharedState->rendererCacheBatches,
      vulkan_renderer_cache_primitive_element_source_type_scene_tree);
  vulkan_batch_instancing_policy instancingPolicy =
      (sharedState->renderState->config->asset.graphicsEnabledInstancing
           ? vulkan_batch_instancing_policy_matching_vertex_attributes
           : vulkan_batch_instancing_policy_no_instancing);
  vulkan_batches_update(sharedState->rendererCacheBatches, instancingPolicy);
}

void vulkan_render_pass_shared_state_send_to_device(vulkan_render_pass_shared_state *sharedState) {
  // No-op.
}

void vulkan_render_pass_shared_state_set_unified_uniform_buffer(
    vulkan_render_pass_shared_state *sharedState, vulkan_global_uniform_buffer_element *global) {

  vulkan_render_pass_camera_state *camera = sharedState->camera;
  vulkan_render_pass_camera_state_set_view_matrix(camera, global->viewMat);
  vulkan_render_pass_camera_state_set_projection_matrix(camera, global->projMat);

  vulkan_render_pass_material_state *materials = sharedState->materials;
  vulkan_render_pass_material_state_set_material_elements(materials, &global->materialCount,
                                                          global->materials);

  vulkan_render_pass_light_state *lights = sharedState->lights;
  vulkan_render_pass_light_state_set_direct_light_elements(
      lights, &global->directionalLightCount, global->directionalLights, &global->pointLightCount,
      global->pointLights, &global->spotLightCount, global->spotLights);

  vulkan_render_pass_skybox_state *skybox = sharedState->skybox;
  vulkan_render_pass_skybox_state_set_skybox_elements(skybox, &global->skybox);

  vulkan_render_pass_font_state *font = sharedState->font;
  vulkan_render_pass_font_state_set_font_elements(font, &global->font);

  global->viewport.width = sharedState->renderState->vks->swapChainExtent.width;
  global->viewport.height = sharedState->renderState->vks->swapChainExtent.height;
}

void vulkan_render_pass_shared_state_debug_print(vulkan_render_pass_shared_state *sharedState,
                                                 int indent) {
  log_debug(INDENT_FORMAT_STRING "shared state:", INDENT_FORMAT_ARGS(0));
  vulkan_batches_debug_print(sharedState->rendererCacheBatches);
  vulkan_render_pass_camera_state_debug_print(sharedState->camera, indent + 2);
  vulkan_render_pass_material_state_debug_print(sharedState->materials, indent + 2);
  vulkan_render_pass_light_state_debug_print(sharedState->lights, indent + 2);
  vulkan_render_pass_skybox_state_debug_print(sharedState->skybox, indent + 2);
  vulkan_render_pass_font_state_debug_print(sharedState->font, indent + 2);
}
