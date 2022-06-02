/* Vulkan G-Buffer.
 */
#pragma once

#include "../../render_state.h"

typedef struct vulkan_render_pass_offscreen_texture_state {
  vulkan_render_state *renderState; ///< Pointer.

  size_t offscreenTextureElementCount;
  const char *offscreenTextureNames[MAX_FRAMEBUFFER_ATTACHMENT_COUNT];
  vulkan_asset_texture *offscreenTextureAssets[MAX_FRAMEBUFFER_ATTACHMENT_COUNT];
  vulkan_textures_texture_element *offscreenTextureElements[MAX_FRAMEBUFFER_ATTACHMENT_COUNT];

} vulkan_render_pass_offscreen_texture_state;

vulkan_render_pass_offscreen_texture_state *
vulkan_render_pass_offscreen_texture_state_create(vulkan_render_state *renderState);
void vulkan_render_pass_offscreen_texture_state_destroy(
    vulkan_render_pass_offscreen_texture_state *gBuffer);
void vulkan_render_pass_offscreen_texture_state_reinit_with_new_swap_chain(
    vulkan_render_pass_offscreen_texture_state *gBuffer);
void vulkan_render_pass_offscreen_texture_state_update(
    vulkan_render_pass_offscreen_texture_state *gBuffer);

uint32_t vulkan_render_pass_offscreen_texture_state_add_offscreen_texture(
    vulkan_render_pass_offscreen_texture_state *gBuffer, const char *name,
    vulkan_image_type imageType);

void vulkan_render_pass_offscreen_texture_state_set_g_buffer_elements(
    vulkan_render_pass_offscreen_texture_state *gBuffer,
    vulkan_offscreen_texture_helper_element *gBufferHelperElement);

void vulkan_render_pass_offscreen_texture_state_debug_print(
    vulkan_render_pass_offscreen_texture_state *gBuffer, int indent);

vulkan_textures_texture_element *vulkan_render_pass_offscreen_texture_state_get_offscreen_texture(
    vulkan_render_pass_offscreen_texture_state *gBuffer, const char *name);
