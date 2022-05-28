/* GLSL shader source code generator */
#pragma once

#include "../../../objects/descriptor.h"
#include "../../../objects/shader_reflect.h"
#include "../../render_state.h"
#include "../render_pass_decls.h"

/// Generates GLSL shader source code.
typedef struct vulkan_render_pass_shader_generator {
  vulkan_render_state *renderState; ///< Pointer.

  UT_string *sourceCode; ///< GLSL source code.
} vulkan_render_pass_shader_generator;

void vulkan_render_pass_shader_generator_init(vulkan_render_pass_shader_generator *shaderGenerator,
                                              vulkan_render_state *renderState);
void vulkan_render_pass_shader_generator_deinit(
    vulkan_render_pass_shader_generator *shaderGenerator);

vulkan_shader *
vulkan_render_pass_shader_generator_get_shader(vulkan_render_pass_shader_generator *shaderGenerator,
                                               vulkan_render_pass_info renderPassInfo,
                                               vulkan_shader_type shaderType);
