/* Shader program */
#pragma once

#include "shader_generator.h"

/// Creates all shaders for all shader stages of the pipeline.
typedef struct vulkan_shader_program {
  /// GLSL shader source code generator.
  vulkan_shader_generator shaderGenerator;

  /// Vertex shader.
  vulkan_shader *vertexShader;

  /// Fragment shader.
  vulkan_shader *fragmentShader;

} vulkan_shader_program;

vulkan_shader_program *vulkan_shader_program_create(vulkan_render_state *renderState);
void vulkan_shader_program_destroy(vulkan_shader_program *shaderProgram);

VkPipelineShaderStageCreateInfo *
vulkan_shader_program_get_shader_stages(vulkan_shader_program *shaderProgram, size_t *count);

void vulkan_shader_program_debug_print(vulkan_shader_program *shaderProgram, int indent);
