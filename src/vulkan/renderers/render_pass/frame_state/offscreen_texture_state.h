/* Vulkan G-Buffer.
 */
#pragma once

#include "../../render_state.h"

typedef struct vulkan_render_pass_offscreen_texture_state {
  vulkan_render_state *renderState; ///< Pointer.

  vulkan_textures_texture_element *gBuffer0TextureElement;
  vulkan_textures_texture_element *gBuffer1TextureElement;
  vulkan_textures_texture_element *gBuffer2TextureElement;

} vulkan_render_pass_offscreen_texture_state;

vulkan_render_pass_offscreen_texture_state *
vulkan_render_pass_offscreen_texture_state_create(vulkan_render_state *renderState);
void vulkan_render_pass_offscreen_texture_state_destroy(
    vulkan_render_pass_offscreen_texture_state *gBuffer);
void vulkan_render_pass_offscreen_texture_state_reinit_with_new_swap_chain(
    vulkan_render_pass_offscreen_texture_state *gBuffer);
void vulkan_render_pass_offscreen_texture_state_update(
    vulkan_render_pass_offscreen_texture_state *gBuffer);

void vulkan_render_pass_offscreen_texture_state_set_g_buffer_elements(
    vulkan_render_pass_offscreen_texture_state *gBuffer,
    vulkan_g_buffer_helper_element *gBufferHelperElement);

void vulkan_render_pass_offscreen_texture_state_debug_print(
    vulkan_render_pass_offscreen_texture_state *gBuffer, int indent);
