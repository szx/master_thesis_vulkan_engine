#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

void update_func(vulkan_renderer *renderer, double dt) {
  log_info("dt: %f", dt);
  if (renderer->vkd->keyboard.esc) {
    // TODO: Move to vulkan_renderer_quit.
    glfwSetWindowShouldClose(renderer->vkd->window, true);
  }
  // HIRO HIRO camera selection
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
