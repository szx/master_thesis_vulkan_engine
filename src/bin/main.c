#include "../data/config.h"
#include "../platform.h"
#include "../vulkan/functions.h"
#include <GLFW/glfw3.h>

int main(int argc, char *argv[]) {
  data_config config = data_config_init();
  GLFWwindow *window;

  // Initialize the library
  if (glfwInit() == GLFW_FALSE) {
    return -1;
  }

  if (glfwVulkanSupported() == GLFW_TRUE) {
    printf("Vulkan supported (%s)\n", VkResult_debug_str(VK_SUCCESS));
  }

  // Create a windowed mode window and its OpenGL context
  window =
      glfwCreateWindow(config.windowWidth, config.windowHeight, str_c_str(&config.windowTitle), NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
    data_config_free(&config);
    return -1;
  }

  // Make the window's context current
  glfwMakeContextCurrent(window);

  //  Loop until the user closes the window
  while (glfwWindowShouldClose(window) == 0) {
    // Render here */
    // glClear(GL_COLOR_BUFFER_BIT);

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
  }

  glfwTerminate();
  data_config_free(&config);
  return 0;
}
