/* GLSL shader generator */
#pragma once

#include "common.h"
#include "descriptor.h"
#include "shader_reflect.h"

/// Describes SPIR-V shader reflection.
typedef struct vulkan_shader_generator {
  // HIRO generate from render state
  int a;
} vulkan_shader_generator;

vulkan_shader_generator *vulkan_shader_generator_create(uint32_t *spvCode, size_t spvSize);
void vulkan_shader_generator_destroy(vulkan_shader_generator *reflect);

void vulkan_shader_generator_debug_print(vulkan_shader_generator *reflect, int indent);
