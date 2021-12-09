#include "../data/config.h"
#include "../platform.h"
#include "../vulkan/vulkan.h"

int main(int argc, char *argv[]) {
  platform_init();
  data_config config = data_config_init();
  vulkan_render_context rctx = vulkan_render_context_init(&config);

  // Load scene.
  platform_path gltfPath =
      get_asset_file_path("triangles", "SimpleMeshes.gltf");
  vulkan_scene scene = parse_gltf_file(gltfPath);
  platform_path_free(&gltfPath);
  vulkan_scene_debug_print(&scene);

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
  vulkan_scene_free(&scene);
  data_config_free(&config);
  platform_free();
  return 0;
}
