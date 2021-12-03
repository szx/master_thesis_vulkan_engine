#include "../platform.h"
#include "../vulkan/functions.h"
#include <GLFW/glfw3.h>

const char *const window_title = "Window title";
const int window_width = 640;
const int window_height = 480;

int main(int argc, char *argv[]) {
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
      glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
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
  return 0;
}
