/* Shader program.
 * Describes all shaders used by render pass.
 */
#pragma once

#include "shader_generator.h"

/// Creates all shaders for all shader stages of the renderPass.
typedef struct vulkan_render_pass_shader_program {
  /// GLSL shader source code generator.
  vulkan_render_pass_shader_generator shaderGenerator;

  /// Vertex shader.
  vulkan_shader *vertexShader;

  /// Fragment shader.
  vulkan_shader *fragmentShader;

} vulkan_render_pass_shader_program;

vulkan_render_pass_shader_program *
vulkan_render_pass_shader_program_create(vulkan_render_state *renderState,
                                         vulkan_render_pass_desc renderPassDesc);
void vulkan_render_pass_shader_program_destroy(vulkan_render_pass_shader_program *shaderProgram);

VkPipelineShaderStageCreateInfo *vulkan_render_pass_shader_program_get_shader_stages(
    vulkan_render_pass_shader_program *shaderProgram, size_t *count);

void vulkan_render_pass_shader_program_debug_print(vulkan_render_pass_shader_program *shaderProgram,
                                                   int indent);
