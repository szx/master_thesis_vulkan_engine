/* Vulkan G-Buffer.
 */
#pragma once

#include "../../render_state.h"

typedef struct render_pass_offscreen_texture_state {
  render_state *renderState; ///< Pointer.

  size_t offscreenTextureElementCount;
  const char *offscreenTextureNames[MAX_FRAMEBUFFER_ATTACHMENT_COUNT];
  asset_texture *offscreenTextureAssets[MAX_FRAMEBUFFER_ATTACHMENT_COUNT];
  textures_texture_element *offscreenTextureElements[MAX_FRAMEBUFFER_ATTACHMENT_COUNT];

} render_pass_offscreen_texture_state;

render_pass_offscreen_texture_state *
render_pass_offscreen_texture_state_create(render_state *renderState);
void render_pass_offscreen_texture_state_destroy(render_pass_offscreen_texture_state *gBuffer);
void render_pass_offscreen_texture_state_reinit_with_new_swap_chain(
    render_pass_offscreen_texture_state *gBuffer);
void render_pass_offscreen_texture_state_update(render_pass_offscreen_texture_state *gBuffer);

uint32_t render_pass_offscreen_texture_state_add_offscreen_texture(
    render_pass_offscreen_texture_state *gBuffer, const char *name, image_type imageType);

void render_pass_offscreen_texture_state_set_g_buffer_elements(
    render_pass_offscreen_texture_state *gBuffer,
    offscreen_texture_helper_element *gBufferHelperElement);

void render_pass_offscreen_texture_state_debug_print(render_pass_offscreen_texture_state *gBuffer,
                                                     int indent);

textures_texture_element *render_pass_offscreen_texture_state_get_offscreen_texture(
    render_pass_offscreen_texture_state *gBuffer, const char *name);
