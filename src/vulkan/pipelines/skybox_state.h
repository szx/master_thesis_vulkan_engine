/* Vulkan skybox.
 */
#pragma once

#include "../common.h"
#include "../objects/render_state.h"

typedef struct vulkan_pipeline_skybox_state {
  vulkan_render_state *renderState; ///< Pointer.

  vulkan_textures_texture_element *cubemapTextureElement;
  float ambientIntensity;
  vulkan_render_cache *boxRenderCache;

} vulkan_pipeline_skybox_state;

vulkan_pipeline_skybox_state *vulkan_pipeline_skybox_state_create(vulkan_render_state *renderState);
void vulkan_pipeline_skybox_state_destroy(vulkan_pipeline_skybox_state *skybox);
void vulkan_pipeline_skybox_state_reinit_with_new_swap_chain(vulkan_pipeline_skybox_state *skybox);

void vulkan_pipeline_skybox_state_set_skybox_elements(vulkan_pipeline_skybox_state *skybox,
                                                      vulkan_skybox_helper_element *skyboxElement);

void vulkan_pipeline_skybox_state_debug_print(vulkan_pipeline_skybox_state *skybox, int indent);
