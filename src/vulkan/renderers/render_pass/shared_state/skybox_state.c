#include "skybox_state.h"

vulkan_render_pass_skybox_state *
vulkan_render_pass_skybox_state_create(vulkan_render_state *renderState) {
  vulkan_render_pass_skybox_state *skybox = core_alloc(sizeof(vulkan_render_pass_skybox_state));

  skybox->renderState = renderState;

  skybox->ambientIntensity = 1.0f;

  return skybox;
}

void vulkan_render_pass_skybox_state_destroy(vulkan_render_pass_skybox_state *skybox) {
  core_free(skybox);
}

void vulkan_render_pass_skybox_state_reinit_with_new_swap_chain(
    vulkan_render_pass_skybox_state *skybox) {
  // No-op.
}

void vulkan_render_pass_skybox_state_update(vulkan_render_pass_skybox_state *skybox) {
  // No-op.
  // TODO: Update skybox parameters (day-light cycle?)
}

void vulkan_render_pass_skybox_state_set_skybox_elements(
    vulkan_render_pass_skybox_state *skybox, vulkan_skybox_helper_element *skyboxElement) {
  skyboxElement->skyboxCubemapTextureId =
      skybox->renderState->rendererCache->skyboxElement->skyboxTextureElement->textureIdx;
}

void vulkan_render_pass_skybox_state_debug_print(vulkan_render_pass_skybox_state *skybox,
                                                 int indent) {
  log_debug(INDENT_FORMAT_STRING "skybox:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "ambientIntensity: %f", INDENT_FORMAT_ARGS(2),
            skybox->ambientIntensity);
}
