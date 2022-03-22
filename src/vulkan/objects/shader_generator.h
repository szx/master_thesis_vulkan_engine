/* GLSL shader generator */
#pragma once

#include "descriptor.h"
#include "render_state.h"
#include "shader_reflect.h"

/// Creates shaders used by pipeline.
typedef struct vulkan_shader_generator {
  // HIRO vulkan_shader_dependencies? and create render pass using it

  /// Vertex shader.
  /// Transforms vertex input before rasterization.
  vulkan_shader *vertexShader;

  /// Fragment shader.
  vulkan_shader *fragmentShader;

} vulkan_shader_generator;

vulkan_shader_generator *vulkan_shader_generator_create(vulkan_render_state *renderState);
void vulkan_shader_generator_destroy(vulkan_shader_generator *shaderGenerator);

VkPipelineShaderStageCreateInfo *
vulkan_shader_generator_get_shader_stages(vulkan_shader_generator *shaderGenerator, size_t *count);

void vulkan_shader_generator_debug_print(vulkan_shader_generator *shaderGenerator, int indent);
