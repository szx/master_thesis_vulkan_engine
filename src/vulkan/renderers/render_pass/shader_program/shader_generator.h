/* GLSL shader source code generator */
#pragma once

#include "../../../objects/descriptor.h"
#include "../../../objects/shader_reflect.h"
#include "../../render_state.h"
#include "../render_pass_decls.h"

/// Generates GLSL shader source code.
typedef struct render_pass_shader_generator {
  render_state *renderState; ///< Pointer.

  UT_string *sourceCode; ///< GLSL source code.
} render_pass_shader_generator;

void render_pass_shader_generator_init(render_pass_shader_generator *shaderGenerator,
                                       render_state *renderState);
void render_pass_shader_generator_deinit(render_pass_shader_generator *shaderGenerator);

shader *render_pass_shader_generator_get_shader(render_pass_shader_generator *shaderGenerator,
                                                render_pass_desc renderPassDesc,
                                                shader_type shaderType);
