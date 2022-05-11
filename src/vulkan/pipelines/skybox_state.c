#include "skybox_state.h"
#include "../objects/render_state.h"

vulkan_pipeline_skybox_state *
vulkan_pipeline_skybox_state_create(vulkan_render_state *renderState) {
  vulkan_pipeline_skybox_state *skybox = core_alloc(sizeof(vulkan_pipeline_skybox_state));

  skybox->renderState = renderState;

  skybox->cubemapTextureElement = vulkan_textures_add_texture(
      skybox->renderState->textures, skybox->renderState->renderCacheList->skybox->cubemapTexture);
  skybox->ambientIntensity = 1.0f;

  // HIRO Refactor manual adding of geometry.
  // HIRO Refactor move boxVertexStreamElement to render state

  // HIRO HIRO added twice during swap chain reinitialization
  skybox->boxRenderCache = vulkan_render_cache_create();
  skybox->boxRenderCache->primitive = core_alloc(sizeof(vulkan_asset_primitive));
  vulkan_asset_primitive_init(skybox->boxRenderCache->primitive, NULL);

  skybox->boxRenderCache->primitive->vertexCount = 36;

  skybox->boxRenderCache->primitive->indices = core_alloc(sizeof(vulkan_asset_vertex_attribute));
  vulkan_asset_vertex_attribute_init(skybox->boxRenderCache->primitive->indices, NULL);
  utarray_resize(skybox->boxRenderCache->primitive->indices->data, 36);
  core_memcpy(utarray_front(skybox->boxRenderCache->primitive->indices->data),
              (uint32_t[36]){0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                             12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                             24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
              utarray_size(skybox->boxRenderCache->primitive->indices->data));

  skybox->boxRenderCache->primitive->positions = core_alloc(sizeof(vulkan_asset_vertex_attribute));
  vulkan_asset_vertex_attribute_init(skybox->boxRenderCache->primitive->positions, NULL);
  utarray_realloc(skybox->boxRenderCache->primitive->positions->data, sizeof(vec3));
  utarray_resize(skybox->boxRenderCache->primitive->positions->data, 36);
  core_memcpy(utarray_front(skybox->boxRenderCache->primitive->positions->data),
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
              utarray_size(skybox->boxRenderCache->primitive->positions->data));

  skybox->boxRenderCache->primitive->normals = core_alloc(sizeof(vulkan_asset_vertex_attribute));
  vulkan_asset_vertex_attribute_init(skybox->boxRenderCache->primitive->normals, NULL);
  skybox->boxRenderCache->primitive->colors = core_alloc(sizeof(vulkan_asset_vertex_attribute));
  vulkan_asset_vertex_attribute_init(skybox->boxRenderCache->primitive->colors, NULL);
  skybox->boxRenderCache->primitive->texCoords = core_alloc(sizeof(vulkan_asset_vertex_attribute));
  vulkan_asset_vertex_attribute_init(skybox->boxRenderCache->primitive->texCoords, NULL);

  vulkan_vertex_stream_add_other(skybox->renderState->vertexStream, skybox->boxRenderCache);

  vulkan_render_cache_list_add_other_render_cache(skybox->renderState->renderCacheList,
                                                  skybox->boxRenderCache);
  return skybox;
}

void vulkan_pipeline_skybox_state_destroy(vulkan_pipeline_skybox_state *skybox) {
  core_free(skybox);
}

void vulkan_pipeline_skybox_state_reinit_with_new_swap_chain(vulkan_pipeline_skybox_state *skybox) {
  // No-op.
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
