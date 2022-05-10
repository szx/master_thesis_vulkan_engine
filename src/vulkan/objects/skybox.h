/* Vulkan skybox.
 */
#pragma once

#include "../common.h"
#include "descriptor.h"
#include "textures.h"
#include "vertex_stream.h"

typedef struct vulkan_skybox {
  vulkan_render_state *renderState; ///< Pointer.

  vulkan_textures_texture_element *cubemapTextureElement;
  float ambientIntensity;
  vulkan_vertex_stream_element boxVertexStreamElement;

} vulkan_skybox;

vulkan_skybox *vulkan_skybox_create(vulkan_render_state *renderState);
void vulkan_skybox_destroy(vulkan_skybox *skybox);

// HIRO HIRO HIRO replace with update_unified_uniform_buffer that syncs uniform buffer
void vulkan_skybox_set_skybox_elements(vulkan_skybox *skybox,
                                       vulkan_skybox_helper_element *skyboxElement);

void vulkan_skybox_debug_print(vulkan_skybox *skybox, int indent);
