/* Vulkan skybox.
 */
#pragma once

#include "../common.h"
#include "../objects/descriptor.h"
#include "../objects/textures.h"
#include "../objects/vertex_stream.h"

typedef struct vulkan_pipeline_skybox_state {
  vulkan_render_state *renderState; ///< Pointer.

  vulkan_textures_texture_element *cubemapTextureElement;
  float ambientIntensity;
  vulkan_vertex_stream_element boxVertexStreamElement;

} vulkan_pipeline_skybox_state;

vulkan_pipeline_skybox_state *vulkan_pipeline_skybox_state_create(vulkan_render_state *renderState);
void vulkan_pipeline_skybox_state_destroy(vulkan_pipeline_skybox_state *skybox);

void vulkan_pipeline_skybox_state_set_skybox_elements(vulkan_pipeline_skybox_state *skybox,
                                                      vulkan_skybox_helper_element *skyboxElement);

void vulkan_pipeline_skybox_state_debug_print(vulkan_pipeline_skybox_state *skybox, int indent);
