#include "../core/platform.h"
#include "../data/config.h"
#include "../vulkan/vulkan.h"

int main(int argc, char *argv[]) {
  platform_init();
  data_config config = data_config_init();
  vulkan_render_context rctx;
  vulkan_render_context_init(&rctx, &config);
  vulkan_render_context_load_scene(&rctx, "triangles");

  // TODO: create vulkan_render_context

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
    vulkan_render_context_draw_frame(&rctx);
  }
  vkDeviceWaitIdle(rctx.vkd->device);

  vulkan_render_context_free(&rctx);
  data_config_free(&config);
  platform_free();
  return 0;
}
