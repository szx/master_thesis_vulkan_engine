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
