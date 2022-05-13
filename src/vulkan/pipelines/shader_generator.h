/* GLSL shader source code generator */
#pragma once

#include "../objects/descriptor.h"
#include "../objects/shader_reflect.h"
#include "../pipelines/pipeline_defs.h"
#include "../renderers/render_state.h"

/// Generates GLSL shader source code.
typedef struct vulkan_pipeline_shader_generator {
  vulkan_render_state *renderState; ///< Pointer.

  UT_string *sourceCode; ///< GLSL source code.
} vulkan_pipeline_shader_generator;

void vulkan_pipeline_shader_generator_init(vulkan_pipeline_shader_generator *shaderGenerator,
                                           vulkan_render_state *renderState);
void vulkan_pipeline_shader_generator_deinit(vulkan_pipeline_shader_generator *shaderGenerator);

vulkan_shader *
vulkan_pipeline_shader_generator_get_shader(vulkan_pipeline_shader_generator *shaderGenerator,
                                            vulkan_pipeline_type pipelineType,
                                            vulkan_shader_type shaderType);
