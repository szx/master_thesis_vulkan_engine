#include "input.h"

input input_default() { return (input){.mouse.sensitivity = 1.0f}; }

void input_clear_released_keys(input *input) {
#define input_key(_name, _glfwKey, ...) input->keyboard.release._name = false;
  VULKAN_INPUT_KEYS(input_key, )
#undef input_key
}
