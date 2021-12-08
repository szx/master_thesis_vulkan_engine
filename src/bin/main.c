#include "../data/config.h"
#include "../platform.h"
#include "../vulkan/vulkan.h"

int main(int argc, char *argv[]) {
  platform_init();
  data_config config = data_config_init();
  vulkan_device vkd = vulkan_device_init(&config);
  vulkan_swap_chain vks = vulkan_swap_chain_init(&vkd);

  // Load scene.
  platform_path gltfPath =
      get_asset_file_path("triangles", "SimpleMeshes.gltf");
  vulkan_scene scene = parse_gltf_file(gltfPath);
  platform_path_free(&gltfPath);
  vulkan_scene_debug_print(&scene);

  // TODO: create vulkan_render_context

  glfwMakeContextCurrent(vkd.window);

  //  Loop until the user closes the window
  while (glfwWindowShouldClose(vkd.window) == 0) {
    // TODO: render vulkan_render_context

    // Swap front and back buffers
    glfwSwapBuffers(vkd.window);

    // Poll for and process events
    glfwPollEvents();
  }

  vulkan_swap_chain_free(&vks);
  vulkan_device_free(&vkd);
  vulkan_scene_free(&scene);
  data_config_free(&config);
  platform_free();
  return 0;
}
