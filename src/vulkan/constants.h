/* Vulkan common constants deemed too impractical to expose in config. */

#pragma once

#define FRAMES_IN_FLIGHT 2
#define MAX_INSTANCE_COUNT 1024
#define MAX_INDIRECT_DRAW_COMMAND_COUNT MAX_INSTANCE_COUNT
#define MAX_MATERIAL_COUNT 128
#define MAX_DIRECTIONAL_LIGHT_COUNT 1
#define MAX_POINT_LIGHT_COUNT 8
#define MIN_DELTA_TIME (1.0 / 60.0)

#define END_OF_VULKAN_SHADER_CONSTANTS
#define VULKAN_SHADER_CONSTANTS(X, ...)                                                            \
  X(FRAMES_IN_FLIGHT, __VA_ARGS__)                                                                 \
  X(MAX_DIRECTIONAL_LIGHT_COUNT, __VA_ARGS__)                                                      \
  X(MAX_POINT_LIGHT_COUNT, __VA_ARGS__)                                                            \
  END_OF_VULKAN_SHADER_CONSTANTS
