/* GLSL shader to SPIR-V binary compiler */

#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include "functions.h"

typedef struct vulkan_shader {
  vulkan_device *vkd; /// vulkan_device pointer.
  platform_path glslPath;
  char *glslCode;
  size_t glslSize;
  shaderc_shader_kind type;
  uint32_t *spvCode;
  size_t spvSize;
  VkShaderModule module;
} vulkan_shader;

void vulkan_shader_init(vulkan_shader *shader, vulkan_device *vkd, platform_path glslPath);
void vulkan_shader_deinit(vulkan_shader *shader);

#include "../codegen/shader.h"

#endif /* !VULKAN_SHADER */
