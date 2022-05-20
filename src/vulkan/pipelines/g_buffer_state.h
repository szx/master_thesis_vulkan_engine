/* Vulkan G-Buffer.
 */
#pragma once

#include "../common.h"
#include "../renderers/render_state.h"

typedef struct vulkan_pipeline_g_buffer_state {
  vulkan_render_state *renderState; ///< Pointer.

  // HIRO Is G-Buffer in flight frame compatible? Add barrier.
  vulkan_textures_texture_element *gBuffer0TextureElement;
  vulkan_textures_texture_element *gBuffer1TextureElement;
  vulkan_textures_texture_element *gBuffer2TextureElement;

  // HIRO Encoding G-Buffer pattern codegen. baseColor needs only 8*3 bits
} vulkan_pipeline_g_buffer_state;

vulkan_pipeline_g_buffer_state *
vulkan_pipeline_g_buffer_state_create(vulkan_render_state *renderState);
void vulkan_pipeline_g_buffer_state_destroy(vulkan_pipeline_g_buffer_state *gBuffer);
void vulkan_pipeline_g_buffer_state_reinit_with_new_swap_chain(
    vulkan_pipeline_g_buffer_state *gBuffer);
void vulkan_pipeline_g_buffer_state_update(vulkan_pipeline_g_buffer_state *gBuffer);

void vulkan_pipeline_g_buffer_state_set_g_buffer_elements(
    vulkan_pipeline_g_buffer_state *gBuffer, vulkan_g_buffer_helper_element *gBufferHelperElement);

void vulkan_pipeline_g_buffer_state_debug_print(vulkan_pipeline_g_buffer_state *gBuffer,
                                                int indent);
