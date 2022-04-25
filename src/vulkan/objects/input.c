#include "input.h"

vulkan_input vulkan_input_default() { return (vulkan_input){.mouse.sensitivity = 1.0f}; }

void vulkan_input_clear_released_keys(vulkan_input *input) {
#define input_key(_name, _glfwKey, ...) input->keyboard.release._name = false;
  VULKAN_INPUT_KEYS(input_key, )
#undef input_key
}
