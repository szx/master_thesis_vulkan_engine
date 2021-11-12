#include "calc.h"
#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>

#include <iostream>

const char *const window_title = "Window title";
const int window_width = 640;
const int window_height = 480;

int main(int argc, char *argv[]) {
  std::cout << "argc: " << argc << std::endl;
  std::cout << "argv[0]: " << argv[0] << std::endl;

  GLFWwindow *window;

  // Initialize the library
  if (glfwInit() == GLFW_FALSE) {
    return -1;
  }

  if (glfwVulkanSupported() == GLFW_TRUE) {
    std::cout << "Vulkan supported" << std::endl;
  }

  // Create a windowed mode window and its OpenGL context
  window =
      glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);
  if (window == nullptr) {
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
