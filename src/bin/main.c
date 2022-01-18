#include "../core/core.h"
#include "../data/data.h"
#include "../vulkan/vulkan.h"

int main(int argc, char *argv[]) {
  platform_create();
  data_config *config = data_config_create();
  data_assets *assets = data_assets_create();
  vulkan_render_context rctx;
  vulkan_render_context_init(&rctx, config, assets, "triangles");
  vulkan_render_context_update_data(&rctx);
  vulkan_render_context_send_to_device(&rctx);

  glfwMakeContextCurrent(rctx.vkd->window);

  //  Loop until the user closes the window
  double currentTime = glfwGetTime();
  while (glfwWindowShouldClose(rctx.vkd->window) == 0) {
    double newTime = glfwGetTime();
    double frameTime = newTime - currentTime;
    currentTime = newTime;
    while (frameTime > 0.0) {
      double dt = MIN(frameTime, MIN_DELTA_TIME);
      // TODO: query input, update state
      frameTime -= dt;
    }
    glfwPollEvents(); // calls GLFW callbacks
    vulkan_render_context_update_data(&rctx);
    vulkan_render_context_send_to_device(&rctx);
    vulkan_render_context_draw_frame(&rctx);
  }
  vkDeviceWaitIdle(rctx.vkd->device);

  vulkan_render_context_deinit(&rctx);
  data_assets_destroy(assets);
  data_config_destroy(config);
  platform_destroy();
  return 0;
}
