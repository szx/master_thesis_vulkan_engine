/* Shader program */
#pragma once

#include "shader_generator.h"

/// Creates all shaders for all shader stages of the pipeline.
typedef struct vulkan_pipeline_shader_program {
  /// GLSL shader source code generator.
  vulkan_pipeline_shader_generator shaderGenerator;

  /// Vertex shader.
  vulkan_shader *vertexShader;

  /// Fragment shader.
  vulkan_shader *fragmentShader;

} vulkan_pipeline_shader_program;

vulkan_pipeline_shader_program *
vulkan_pipeline_shader_program_create(vulkan_render_state *renderState,
                                      vulkan_pipeline_info pipelineInfo);
void vulkan_pipeline_shader_program_destroy(vulkan_pipeline_shader_program *shaderProgram);

VkPipelineShaderStageCreateInfo *
vulkan_pipeline_shader_program_get_shader_stages(vulkan_pipeline_shader_program *shaderProgram,
                                                 size_t *count);

void vulkan_pipeline_shader_program_debug_print(vulkan_pipeline_shader_program *shaderProgram,
                                                int indent);
