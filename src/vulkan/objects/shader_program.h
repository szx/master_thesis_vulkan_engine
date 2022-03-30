/* GLSL shader generator */
#pragma once

#include "descriptor.h"
#include "render_state.h"
#include "shader_reflect.h"

/// Generates GLSL shader source code.
/// Creates all shaders for all shader stages of the pipeline.
typedef struct vulkan_shader_program {
  /// Vertex shader.
  /// Transforms vertex input before rasterization.
  vulkan_shader *vertexShader;

  /// Fragment shader.
  vulkan_shader *fragmentShader;

} vulkan_shader_program;

vulkan_shader_program *vulkan_shader_program_create(vulkan_render_state *renderState);
void vulkan_shader_program_destroy(vulkan_shader_program *shaderProgram);

VkPipelineShaderStageCreateInfo *
vulkan_shader_program_get_shader_stages(vulkan_shader_program *shaderProgram, size_t *count);

void vulkan_shader_program_debug_print(vulkan_shader_program *shaderProgram, int indent);

/* GLSL shader source generation */

void glsl_add_header(UT_string *s);

void glsl_add_vertex_shader_input_variables_defines(UT_string *s, vulkan_vertex_stream *stream);

void glsl_add_defines(UT_string *s, vulkan_render_state *renderState);

void glsl_add_vertex_shader_input_variables(UT_string *s, vulkan_vertex_stream *stream);

void glsl_add_vertex_shader_output_variables(UT_string *s);

void glsl_add_fragment_shader_input_variables(UT_string *s);

void glsl_add_fragment_shader_output_variables(UT_string *s);

void glsl_add_uniform_buffers(UT_string *s, vulkan_unified_uniform_buffer *unifiedUniformBuffer);

void glsl_add_entry_point_begin(UT_string *s);

void glsl_add_entry_point_end(UT_string *s);

void glsl_add_body(UT_string *s, vulkan_shader_type type);
