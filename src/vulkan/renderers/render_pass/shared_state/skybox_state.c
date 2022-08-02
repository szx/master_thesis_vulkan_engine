#include "skybox_state.h"

render_pass_skybox_state *render_pass_skybox_state_create(render_state *renderState) {
  render_pass_skybox_state *skybox = core_alloc(sizeof(render_pass_skybox_state));

  skybox->renderState = renderState;

  skybox->ambientIntensity = 1.0f;

  return skybox;
}

void render_pass_skybox_state_destroy(render_pass_skybox_state *skybox) { core_free(skybox); }

void render_pass_skybox_state_reinit_with_new_swap_chain(render_pass_skybox_state *skybox) {
  // No-op.
}

void render_pass_skybox_state_update(render_pass_skybox_state *skybox) {
  // No-op.
  // TODO: Update skybox parameters (day-light cycle?)
}

void render_pass_skybox_state_set_skybox_elements(render_pass_skybox_state *skybox,
                                                  skybox_helper_element *skyboxElement) {
  skyboxElement->skyboxCubemapTextureId =
      skybox->renderState->rendererCache->skyboxElement->skyboxTextureElement->textureIdx;
}

void render_pass_skybox_state_debug_print(render_pass_skybox_state *skybox, int indent) {
  log_debug(INDENT_FORMAT_STRING "skybox:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "ambientIntensity: %f", INDENT_FORMAT_ARGS(2),
            skybox->ambientIntensity);
}
