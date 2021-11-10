#define GLFW_INCLUDE_VULKAN
#include "calc.h"
#include <GLFW/glfw3.h>

#include <iostream>

static const char *windowTitle = "Window title";
int windowWidth = 640;
int windowHeight = 480;

int main() {
  GLFWwindow *window;

  /* Initialize the library */
  if (glfwInit() == GLFW_FALSE) {
    return -1;
  }

  if (glfwVulkanSupported() == GLFW_TRUE) {
    std::cout << "Vulkan supported" << std::endl;
  }

  /* Create a windowed mode window and its OpenGL context */
  window =
      glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
  if (window == nullptr) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  /* Loop until the user closes the window */
  while (glfwWindowShouldClose(window) == 0) {
    /* Render here */
    // glClear(GL_COLOR_BUFFER_BIT);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
