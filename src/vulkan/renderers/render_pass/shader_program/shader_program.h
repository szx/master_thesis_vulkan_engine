/* Shader program.
 * Describes all shaders used by render pass.
 */
#pragma once

#include "shader_generator.h"

/// Creates all shaders for all shader stages of the renderPass.
typedef struct render_pass_shader_program {
  /// GLSL shader source code generator.
  render_pass_shader_generator shaderGenerator;

  /// Vertex shader.
  shader *vertexShader;

  /// Fragment shader.
  shader *fragmentShader;

} render_pass_shader_program;

render_pass_shader_program *render_pass_shader_program_create(render_state *renderState,
                                                              render_pass_desc renderPassDesc);
void render_pass_shader_program_destroy(render_pass_shader_program *shaderProgram);

VkPipelineShaderStageCreateInfo *
render_pass_shader_program_get_shader_stages(render_pass_shader_program *shaderProgram,
                                             size_t *count);

void render_pass_shader_program_debug_print(render_pass_shader_program *shaderProgram, int indent);
