/* GLFW input handling */

#pragma once

#define END_OF_VULKAN_INPUT_KEYS
#define VULKAN_INPUT_KEYS(X, ...)                                                                  \
  X(w, GLFW_KEY_W, __VA_ARGS__)                                                                    \
  X(a, GLFW_KEY_A, __VA_ARGS__)                                                                    \
  X(s, GLFW_KEY_S, __VA_ARGS__)                                                                    \
  X(d, GLFW_KEY_D, __VA_ARGS__)                                                                    \
  X(q, GLFW_KEY_Q, __VA_ARGS__)                                                                    \
  X(e, GLFW_KEY_E, __VA_ARGS__)                                                                    \
  X(i, GLFW_KEY_I, __VA_ARGS__)                                                                    \
  X(o, GLFW_KEY_O, __VA_ARGS__)                                                                    \
  X(p, GLFW_KEY_P, __VA_ARGS__)                                                                    \
  X(t, GLFW_KEY_T, __VA_ARGS__)                                                                    \
  X(tab, GLFW_KEY_TAB, __VA_ARGS__)                                                                \
  X(num1, GLFW_KEY_1, __VA_ARGS__)                                                                 \
  X(num2, GLFW_KEY_2, __VA_ARGS__)                                                                 \
  X(num3, GLFW_KEY_3, __VA_ARGS__)                                                                 \
  X(num4, GLFW_KEY_4, __VA_ARGS__)                                                                 \
  X(num6, GLFW_KEY_6, __VA_ARGS__)                                                                 \
  X(num7, GLFW_KEY_7, __VA_ARGS__)                                                                 \
  X(num8, GLFW_KEY_8, __VA_ARGS__)                                                                 \
  X(num9, GLFW_KEY_9, __VA_ARGS__)                                                                 \
  X(esc, GLFW_KEY_ESCAPE, __VA_ARGS__)                                                             \
  X(leftBracket, GLFW_KEY_LEFT_BRACKET, __VA_ARGS__)                                               \
  X(rightBracket, GLFW_KEY_RIGHT_BRACKET, __VA_ARGS__)                                             \
  X(leftShift, GLFW_KEY_LEFT_SHIFT, __VA_ARGS__)                                                   \
  END_OF_VULKAN_INPUT_KEYS

typedef struct input {
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
    double x, y;
    double lastX, lastY;
  } mouse; ///< GLFW mouse input.
} input;

input input_default();

void input_clear_released_keys(input *input);
