#include "skybox_state.h"
#include "../renderers/render_state.h"

vulkan_pipeline_skybox_state *
vulkan_pipeline_skybox_state_create(vulkan_render_state *renderState) {
  vulkan_pipeline_skybox_state *skybox = core_alloc(sizeof(vulkan_pipeline_skybox_state));

  skybox->renderState = renderState;

  skybox->cubemapTextureElement = vulkan_textures_add_texture(
      skybox->renderState->textures, skybox->renderState->rendererCache->skybox->cubemapTexture);
  skybox->ambientIntensity = 1.0f;

  skybox->boxRenderCache = vulkan_renderer_cache_primitive_element_create_from_geometry(
      true, GLM_MAT4_IDENTITY, 36,
      (uint32_t[36]){0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
                     18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
      (vec3[36]){// -X side,
                 {-1, -1, 1},
                 {-1, -1, -1},
                 {-1, 1, -1},
                 {-1, 1, -1},
                 {-1, 1, 1},
                 {-1, -1, 1},
                 // +X side
                 {1, -1, -1},
                 {1, -1, 1},
                 {1, 1, 1},
                 {1, 1, 1},
                 {1, 1, -1},
                 {1, -1, -1},
                 // -Y side
                 {-1, -1, -1},
                 {-1, -1, 1},
                 {1, -1, -1},
                 {1, -1, -1},
                 {-1, -1, 1},
                 {1, -1, 1},
                 // +Y side
                 {-1, 1, -1},
                 {1, 1, -1},
                 {1, 1, 1},
                 {1, 1, 1},
                 {-1, 1, 1},
                 {-1, 1, -1},
                 // -Z side
                 {-1, 1, -1},
                 {-1, -1, -1},
                 {1, -1, -1},
                 {1, -1, -1},
                 {1, 1, -1},
                 {-1, 1, -1},
                 // +Z side
                 {-1, -1, 1},
                 {-1, 1, 1},
                 {1, 1, 1},
                 {1, 1, 1},
                 {1, -1, 1},
                 {-1, -1, 1}},
      NULL, NULL, NULL);

  skybox->boxRenderCache->vertexStreamElement = vulkan_vertex_stream_add_geometry(
      skybox->renderState->vertexStream, skybox->boxRenderCache->primitive->vertexCount,
      skybox->boxRenderCache->primitive->indices->data,
      skybox->boxRenderCache->primitive->positions->data,
      skybox->boxRenderCache->primitive->normals->data,
      skybox->boxRenderCache->primitive->colors->data,
      skybox->boxRenderCache->primitive->texCoords->data);

  return skybox;
}

void vulkan_pipeline_skybox_state_destroy(vulkan_pipeline_skybox_state *skybox) {
  vulkan_renderer_cache_primitive_element_destroy(skybox->boxRenderCache);
  core_free(skybox);
}

void vulkan_pipeline_skybox_state_reinit_with_new_swap_chain(vulkan_pipeline_skybox_state *skybox) {
  // No-op.
}

void vulkan_pipeline_skybox_state_update(vulkan_pipeline_skybox_state *skybox) {
  // No-op.
  // TODO: Update skybox parameters (day-light cycle?)
}

void vulkan_pipeline_skybox_state_set_skybox_elements(vulkan_pipeline_skybox_state *skybox,
                                                      vulkan_skybox_helper_element *skyboxElement) {
  skyboxElement->skyboxCubemapTextureId = skybox->cubemapTextureElement->textureIdx;
}

void vulkan_pipeline_skybox_state_debug_print(vulkan_pipeline_skybox_state *skybox, int indent) {
  log_debug(INDENT_FORMAT_STRING "skybox:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "ambientIntensity: %f", INDENT_FORMAT_ARGS(2),
            skybox->ambientIntensity);
}
