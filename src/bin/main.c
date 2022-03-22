#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

int main(int argc, char *argv[]) {
  platform_create();
  data_config *config = data_config_create();
  data_asset_db *assetDb = data_asset_db_create();
  vulkan_device *vkd = vulkan_device_create(config, assetDb);
  vulkan_swap_chain *vks = vulkan_swap_chain_create(vkd);
  vulkan_renderer *renderer =
      vulkan_renderer_create(config, assetDb, vks, config->settingsStartScene);
  vulkan_renderer_update(renderer);
  vulkan_renderer_send_to_device(renderer);
  vulkan_renderer_debug_print(renderer);

  glfwMakeContextCurrent(renderer->vkd->window);

  //  Loop until the user closes the window
  double currentTime = glfwGetTime();
  while (glfwWindowShouldClose(renderer->vkd->window) == 0) {
    double newTime = glfwGetTime();
    double frameTime = newTime - currentTime;
    currentTime = newTime;
    while (frameTime > 0.0) {
      double dt = MIN(frameTime, MIN_DELTA_TIME);
      // TODO: query input, update state
      frameTime -= dt;
    }
    glfwPollEvents(); // calls GLFW callbacks
    vulkan_renderer_update(renderer);
    vulkan_renderer_send_to_device(renderer);
    vulkan_renderer_draw_frame(renderer);
  }
  vkDeviceWaitIdle(renderer->vkd->device);

  vulkan_renderer_destroy(renderer);
  data_asset_db_destroy(assetDb);
  data_config_destroy(config);
  platform_destroy();
  return 0;
}
