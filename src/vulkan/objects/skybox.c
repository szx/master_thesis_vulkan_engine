#include "skybox.h"
#include "render_state.h"

vulkan_skybox *vulkan_skybox_create(vulkan_render_state *renderState) {
  vulkan_skybox *skybox = core_alloc(sizeof(vulkan_skybox));

  skybox->renderState = renderState;
  skybox->cubemapTextureElement = vulkan_textures_add_texture(
      skybox->renderState->textures, skybox->renderState->renderCacheList->skybox->cubemapTexture);
  skybox->ambientIntensity = 1.0f;

  // HIRO: Refactor manual adding of geometry.
  UT_array *indices;
  utarray_alloc(indices, sizeof(uint32_t));
  utarray_resize(indices, 36);
  core_memcpy(utarray_front(indices),
              (uint32_t[36]){0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                             12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                             24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
              utarray_size(indices));
  UT_array *positions;
  utarray_alloc(positions, sizeof(vec3));
  utarray_resize(positions, 36);
  core_memcpy(utarray_front(positions),
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
              utarray_size(positions));

  skybox->boxVertexStreamElement = vulkan_vertex_stream_add_geometry(
      renderState->vertexStream, 36, indices, positions, NULL, NULL, NULL);
  utarray_free(indices);
  utarray_free(positions);
  return skybox;
}

void vulkan_skybox_destroy(vulkan_skybox *skybox) { core_free(skybox); }

void vulkan_skybox_set_skybox_elements(vulkan_skybox *skybox,
                                       vulkan_skybox_helper_element *skyboxElement) {
  skyboxElement->skyboxCubemapTextureId = skybox->cubemapTextureElement->textureIdx;
}

void vulkan_skybox_debug_print(vulkan_skybox *skybox, int indent) {
  log_debug(INDENT_FORMAT_STRING "skybox:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "ambientIntensity: %f", INDENT_FORMAT_ARGS(2),
            skybox->ambientIntensity);
}
