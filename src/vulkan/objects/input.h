/* GLFW input handling */

#pragma once

#define END_OF_VULKAN_INPUT_KEYS
#define VULKAN_INPUT_KEYS(X, ...)                                                                  \
  X(w, GLFW_KEY_W, __VA_ARGS__)                                                                    \
  X(a, GLFW_KEY_A, __VA_ARGS__)                                                                    \
  X(s, GLFW_KEY_S, __VA_ARGS__)                                                                    \
  X(d, GLFW_KEY_D, __VA_ARGS__)                                                                    \
  X(q, GLFW_KEY_Q, __VA_ARGS__)                                                                    \
  X(num1, GLFW_KEY_W, __VA_ARGS__)                                                                 \
  X(num2, GLFW_KEY_W, __VA_ARGS__)                                                                 \
  X(num3, GLFW_KEY_W, __VA_ARGS__)                                                                 \
  X(num4, GLFW_KEY_W, __VA_ARGS__)                                                                 \
  X(esc, GLFW_KEY_ESCAPE, __VA_ARGS__)                                                             \
  X(leftBracket, GLFW_KEY_LEFT_BRACKET, __VA_ARGS__)                                               \
  X(rightBracket, GLFW_KEY_RIGHT_BRACKET, __VA_ARGS__)                                             \
  END_OF_VULKAN_INPUT_KEYS

typedef struct vulkan_input {
  struct {
    struct {
#define input_key(_name, ...) bool _name;
      VULKAN_INPUT_KEYS(input_key, )
#undef input_key
    } press;
    struct {
#define input_key(_name, ...) bool _name;
      VULKAN_INPUT_KEYS(input_key, )
#undef input_key
    } release;
  } keyboard; ///< GLFW keyboard input.

  struct {
    float sensitivity;
    double xPos, yPos;
    double xLastPos, yLastPos;
  } mouse; ///< GLFW mouse input.
} vulkan_input;

void vulkan_input_clear_released_keys(vulkan_input *input);
