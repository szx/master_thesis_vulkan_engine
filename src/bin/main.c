#include "../data/config.h"
#include "../platform.h"
#include "../vulkan/vulkan.h"
#include <GLFW/glfw3.h>

int main(int argc, char *argv[]) {
  platform_init();
  data_config config = data_config_init();
  GLFWwindow *window;

  // Initialize the library
  if (glfwInit() == GLFW_FALSE) {
    return -1;
  }

  if (glfwVulkanSupported() == GLFW_TRUE) {
    log_info("Vulkan supported (%s)\n", VkResult_debug_str(VK_SUCCESS));
  }

  // Load scene.
  platform_path gltfPath =
      get_asset_file_path("triangles", "SimpleMeshes.gltf");
  vulkan_scene scene = parse_gltf_file(gltfPath);
  platform_path_free(&gltfPath);
  vulkan_scene_debug_print(&scene);

  // TODO: create vulkan_render_context

  // Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(config.windowWidth, config.windowHeight,
                            str_c_str(&config.windowTitle), NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
    data_config_free(&config);
    return -1;
  }

  // Make the window's context current
  glfwMakeContextCurrent(window);

  //  Loop until the user closes the window
  while (glfwWindowShouldClose(window) == 0) {
    // TODO: render vulkan_render_context

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
  }

  glfwTerminate();
  vulkan_scene_free(&scene);
  data_config_free(&config);
  platform_free();
  return 0;
}
