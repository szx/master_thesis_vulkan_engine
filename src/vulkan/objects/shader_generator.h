/* GLSL shader source code generator */
#pragma once

#include "descriptor.h"
#include "pipeline_impl.h"
#include "render_state.h"
#include "shader_reflect.h"

/// Generates GLSL shader source code.
typedef struct vulkan_shader_generator {
  vulkan_render_state *renderState; ///< Pointer.

  UT_string *sourceCode; ///< GLSL source code.
} vulkan_shader_generator;

void vulkan_shader_generator_init(vulkan_shader_generator *shaderGenerator,
                                  vulkan_render_state *renderState);
void vulkan_shader_generator_deinit(vulkan_shader_generator *shaderGenerator);

vulkan_shader *vulkan_shader_generator_get_shader(vulkan_shader_generator *shaderGenerator,
                                                  vulkan_pipeline_type pipelineType,
                                                  vulkan_shader_type shaderType);
