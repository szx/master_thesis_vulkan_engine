#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

void update_func(vulkan_renderer *renderer, double dt) {
  vulkan_device *vkd = renderer->vkd;
  vulkan_render_state *renderState = renderer->renderState;
  if (renderer->vkd->input.keyboard.release.esc) {
    // TODO: Move to vulkan_renderer_quit.
    glfwSetWindowShouldClose(renderer->vkd->window, true);
  }

  if (renderer->vkd->input.keyboard.release.leftBracket) {
    size_t cameraIdx = renderState->camera->cameraIdx - 1;
    if (renderState->camera->cameraIdx == 0) {
      cameraIdx = utarray_len(renderState->camera->renderCacheList->cameraRenderCaches) - 1;
    }
    vulkan_camera_select(renderState->camera, cameraIdx);
  }
  if (renderer->vkd->input.keyboard.release.rightBracket) {
    vulkan_camera_select(renderState->camera, renderState->camera->cameraIdx + 1);
  }

  if (renderer->vkd->input.keyboard.release.l) {
    vec3 position = GLM_VEC3_ONE_INIT;
    // TODO: Refactor getting camera position out of camera view matrix.
    mat4 viewMat;
    vulkan_camera_set_view_matrix(renderState->camera, viewMat);
    mat4 viewMatInv;
    glm_mat4_inv(viewMat, viewMatInv);
    glm_vec3_copy(viewMatInv[3], position);
    vec3 color = GLM_VEC3_ONE_INIT;
    float radius = 10.0f;
    vulkan_lights_add(renderState->lights, position, color, radius);
  }

  if (renderer->vkd->input.keyboard.press.w) {
    glm_translate_z(renderState->camera->userTransform, 1.0f * dt);
  }
  if (renderer->vkd->input.keyboard.press.s) {
    glm_translate_z(renderState->camera->userTransform, -1.0f * dt);
  }
  if (renderer->vkd->input.keyboard.press.d) {
    glm_translate_x(renderState->camera->userTransform, 1.0f * dt);
  }
  if (renderer->vkd->input.keyboard.press.a) {
    glm_translate_x(renderState->camera->userTransform, -1.0f * dt);
  }
  if (renderer->vkd->input.keyboard.press.q) {
    glm_translate_y(renderState->camera->userTransform, 1.0f * dt);
  }
  if (renderer->vkd->input.keyboard.press.e) {
    glm_translate_y(renderState->camera->userTransform, -1.0f * dt);
  }
}

int main(int argc, char *argv[]) {
  platform_create();
  data_config *config = data_config_create();
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);
  vulkan_swap_chain *vks = vulkan_swap_chain_create(vkd);
  vulkan_renderer *renderer =
      vulkan_renderer_create(config, assetDb, vks, config->settingsStartScene);

  vulkan_renderer_run_main_loop(renderer, update_func);

  vulkan_renderer_destroy(renderer);
  vulkan_swap_chain_destroy(vks);
  vulkan_device_destroy(vkd);
  data_asset_db_destroy(assetDb);
  data_config_destroy(config);
  platform_destroy();
  return 0;
}
