/* Vulkan common constants deemed too impractical to expose in config. */

#pragma once

#define FRAMES_IN_FLIGHT 2
#define MAX_OFFSCREEN_TEXTURE_COUNT 16
#define MAX_RENDER_TARGET_COUNT 8
#define MAX_FRAMEBUFFER_ATTACHMENT_COUNT (MAX_RENDER_TARGET_COUNT + 1 + 1)
#define MAX_INDIRECT_DRAW_COMMAND_COUNT 1024
#define MAX_MATERIAL_COUNT 128
#define MAX_DIRECTIONAL_LIGHT_COUNT 1
#define MAX_POINT_LIGHT_COUNT 128
#define MAX_SPOT_LIGHT_COUNT 8
#define MAX_TEXT_CHARACTER_COUNT 256
#define MIN_DELTA_TIME (1.0 / 60.0)
#define WORLD_UP                                                                                   \
  (vec3) { 0, 1, 0 }

#define END_OF_VULKAN_SHADER_CONSTANTS
#define VULKAN_SHADER_CONSTANTS(X, ...)                                                            \
  X(FRAMES_IN_FLIGHT, __VA_ARGS__)                                                                 \
  X(MAX_OFFSCREEN_TEXTURE_COUNT, __VA_ARGS__)                                                      \
  X(MAX_MATERIAL_COUNT, __VA_ARGS__)                                                               \
  X(MAX_DIRECTIONAL_LIGHT_COUNT, __VA_ARGS__)                                                      \
  X(MAX_POINT_LIGHT_COUNT, __VA_ARGS__)                                                            \
  X(MAX_SPOT_LIGHT_COUNT, __VA_ARGS__)                                                             \
  X(MAX_TEXT_CHARACTER_COUNT, __VA_ARGS__)                                                         \
  END_OF_VULKAN_SHADER_CONSTANTS
