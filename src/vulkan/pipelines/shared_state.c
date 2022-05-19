#include "shared_state.h"

void vulkan_pipeline_shared_state_init(vulkan_pipeline_shared_state *sharedState,
                                       vulkan_render_state *renderState) {
  sharedState->renderState = renderState;

  sharedState->camera = vulkan_pipeline_camera_state_create(sharedState->renderState);
  sharedState->materials = vulkan_pipeline_material_state_create(sharedState->renderState);
  sharedState->lights = vulkan_pipeline_light_state_create(sharedState->renderState);
  sharedState->skybox = vulkan_pipeline_skybox_state_create(sharedState->renderState);
  sharedState->font = vulkan_pipeline_font_state_create(sharedState->renderState);
  sharedState->rendererCacheBatches = vulkan_batches_create(sharedState->renderState->vkd);

  sharedState->depthBufferImage = vulkan_image_create(
      sharedState->renderState->vkd, vulkan_image_type_depth_buffer,
      sharedState->renderState->vks->swapChainExtent.width,
      sharedState->renderState->vks->swapChainExtent.height, VK_FORMAT_UNDEFINED);
}

void vulkan_pipeline_shared_state_deinit(vulkan_pipeline_shared_state *sharedState) {
  vulkan_batches_destroy(sharedState->rendererCacheBatches);
  vulkan_pipeline_font_state_destroy(sharedState->font);
  vulkan_pipeline_skybox_state_destroy(sharedState->skybox);
  vulkan_pipeline_light_state_destroy(sharedState->lights);
  vulkan_pipeline_material_state_destroy(sharedState->materials);
  vulkan_pipeline_camera_state_destroy(sharedState->camera);

  vulkan_image_destroy(sharedState->depthBufferImage);
}

void vulkan_pipeline_shared_state_reinit_with_new_swap_chain(
    vulkan_pipeline_shared_state *sharedState) {

  vulkan_pipeline_font_state_reinit_with_new_swap_chain(sharedState->font);
  vulkan_pipeline_skybox_state_reinit_with_new_swap_chain(sharedState->skybox);
  vulkan_pipeline_light_state_reinit_with_new_swap_chain(sharedState->lights);
  vulkan_pipeline_material_state_reinit_with_new_swap_chain(sharedState->materials);
  vulkan_pipeline_camera_state_reinit_with_new_swap_chain(sharedState->camera);

  vulkan_image_destroy(sharedState->depthBufferImage);
  sharedState->depthBufferImage = vulkan_image_create(
      sharedState->renderState->vkd, vulkan_image_type_depth_buffer,
      sharedState->renderState->vks->swapChainExtent.width,
      sharedState->renderState->vks->swapChainExtent.height, VK_FORMAT_UNDEFINED);
}

void vulkan_pipeline_shared_state_update(vulkan_pipeline_shared_state *sharedState) {
  log_debug("updating pipeline shared state");

  vulkan_pipeline_camera_state_update(sharedState->camera);
  vulkan_pipeline_material_state_update(sharedState->materials);
  vulkan_pipeline_light_state_update(sharedState->lights);
  vulkan_pipeline_skybox_state_update(sharedState->skybox);
  vulkan_pipeline_font_state_update(sharedState->font);

  vulkan_renderer_cache_add_new_primitive_elements_to_batches(
      sharedState->renderState->rendererCache, sharedState->rendererCacheBatches,
      vulkan_renderer_cache_primitive_element_source_type_scene_tree);
  vulkan_batch_instancing_policy instancingPolicy =
      (sharedState->renderState->config->asset.graphicsEnabledInstancing
           ? vulkan_batch_instancing_policy_matching_vertex_attributes
           : vulkan_batch_instancing_policy_no_instancing);
  vulkan_batches_update(sharedState->rendererCacheBatches, instancingPolicy);
}

void vulkan_pipeline_shared_state_send_to_device(vulkan_pipeline_shared_state *sharedState) {
  vulkan_image_send_to_device(sharedState->depthBufferImage);
}

void vulkan_pipeline_shared_state_set_unified_uniform_buffer(
    vulkan_pipeline_shared_state *sharedState, vulkan_global_uniform_buffer_element *global) {

  vulkan_pipeline_camera_state *camera = sharedState->camera;
  vulkan_pipeline_camera_state_set_view_matrix(camera, global->viewMat);
  vulkan_pipeline_camera_state_set_projection_matrix(camera, global->projMat);

  vulkan_pipeline_material_state *materials = sharedState->materials;
  vulkan_pipeline_material_state_set_material_elements(materials, &global->materialCount,
                                                       global->materials);

  vulkan_pipeline_light_state *lights = sharedState->lights;
  vulkan_pipeline_light_state_set_directional_light_elements(lights, &global->directionalLightCount,
                                                             global->directionalLights);
  vulkan_pipeline_light_state_set_point_light_elements(lights, &global->pointLightCount,
                                                       global->pointLights);

  vulkan_pipeline_skybox_state *skybox = sharedState->skybox;
  vulkan_pipeline_skybox_state_set_skybox_elements(skybox, &global->skybox);

  vulkan_pipeline_font_state *font = sharedState->font;
  vulkan_pipeline_font_state_set_font_elements(font, &global->font);

  global->viewport.width = sharedState->renderState->vks->swapChainExtent.width;
  global->viewport.height = sharedState->renderState->vks->swapChainExtent.height;
}

void vulkan_pipeline_shared_state_debug_print(vulkan_pipeline_shared_state *sharedState,
                                              int indent) {
  log_debug(INDENT_FORMAT_STRING "shared state:", INDENT_FORMAT_ARGS(0));
  vulkan_batches_debug_print(sharedState->rendererCacheBatches);
  vulkan_pipeline_camera_state_debug_print(sharedState->camera, indent + 2);
  vulkan_pipeline_material_state_debug_print(sharedState->materials, indent + 2);
  vulkan_pipeline_light_state_debug_print(sharedState->lights, indent + 2);
  vulkan_pipeline_skybox_state_debug_print(sharedState->skybox, indent + 2);
  vulkan_pipeline_font_state_debug_print(sharedState->font, indent + 2);
  vulkan_image_debug_print(sharedState->depthBufferImage, indent + 2);
}
