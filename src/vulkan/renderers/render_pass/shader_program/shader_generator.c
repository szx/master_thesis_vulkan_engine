#include "shader_generator.h"

void render_pass_shader_generator_init(render_pass_shader_generator *shaderGenerator,
                                       render_state *renderState) {
  shaderGenerator->renderState = renderState;
  utstring_new(shaderGenerator->sourceCode);
}

void render_pass_shader_generator_deinit(render_pass_shader_generator *shaderGenerator) {
  utstring_free(shaderGenerator->sourceCode);
}

/* GLSL helper functions */

void glsl_add_header(UT_string *s) {
  utstring_printf(s, "#version 450\n");
  utstring_printf(s, "#extension GL_EXT_nonuniform_qualifier : require\n");
  utstring_printf(s, "#extension GL_EXT_scalar_block_layout : require\n");
#if defined(DEBUG)
  utstring_printf(s, "#extension GL_EXT_debug_printf : require\n");
  utstring_printf(s, "#define DEBUG_PRINTF 1\n");
#endif
}

void glsl_add_common_source(UT_string *s) {
  UT_string *commonGlslSource = read_text_file(globals.assetsShaderCommonFilepath);
  utstring_printf(s, "%s\n", utstring_body(commonGlslSource));
  utstring_free(commonGlslSource);
}

void glsl_add_vertex_shader_input_variables_defines(UT_string *s, vertex_stream *stream) {
  assert(stream->attributes > 0);
  size_t attributes = stream->attributes;

  if ((attributes & vertex_attribute_type_position) != 0) {
    utstring_printf(s, "#define IN_POSITION 1\n");
  }
  if ((attributes & vertex_attribute_type_normal) != 0) {
    utstring_printf(s, "#define IN_NORMAL 1\n");
  }
  if ((attributes & vertex_attribute_type_color) != 0) {
    utstring_printf(s, "#define IN_COLOR 1\n");
  }
  if ((attributes & vertex_attribute_type_texcoord) != 0) {
    utstring_printf(s, "#define IN_TEXCOORD 1\n");
  }
  if ((attributes & vertex_attribute_type_tangent) != 0) {
    utstring_printf(s, "#define IN_TANGENT 1\n");
  }
}

void glsl_add_shader_type_defines(UT_string *s, shader_type shaderType) {
  if (shaderType == shader_type_vertex) {
    utstring_printf(s, "#define SHADER_VERTEX 1\n");
  } else if (shaderType == shader_type_fragment) {
    utstring_printf(s, "#define SHADER_FRAGMENT 1\n");
  } else {
    UNREACHABLE;
  }
}

void glsl_add_offscreen_texture_defines(UT_string *s, render_pass_desc renderPassDesc) {
  for (size_t i = 0; i < renderPassDesc.offscreenFragmentShaderInputCount; i++) {
    assert(renderPassDesc.offscreenFragmentShaderInputs[i].name != NULL);
    utstring_printf(s, "#define OFFSCREEN_TEXTURE_%s 1\n",
                    renderPassDesc.offscreenFragmentShaderInputs[i].name);
  }
}

void glsl_add_defines(UT_string *s, render_pass_desc renderPassDesc, shader_type shaderType,
                      render_state *renderState) {
  glsl_add_vertex_shader_input_variables_defines(s, renderState->vertexStream);
  glsl_add_shader_type_defines(s, shaderType);
#define x(_name, ...) utstring_printf(s, "#define " #_name " " MACRO_STR((_name)) "\n");
  CODEGEN_CONSTANTS(x, )
#undef x
  glsl_add_offscreen_texture_defines(s, renderPassDesc);
}

void glsl_add_vertex_shader_input_variables(UT_string *s, vertex_stream *stream) {
  assert(stream->attributes > 0);
  size_t attributes = stream->attributes;

  uint32_t location = 0;
  if ((attributes & vertex_attribute_type_position) != 0) {
    utstring_printf(s, "layout(location = %u) in vec3 inPosition;\n", location);
    location++;
  }
  if ((attributes & vertex_attribute_type_normal) != 0) {
    utstring_printf(s, "layout(location = %u) in vec3 inNormal;\n", location);
    location++;
  }
  if ((attributes & vertex_attribute_type_color) != 0) {
    utstring_printf(s, "layout(location = %u) in vec3 inColor;\n", location);
    location++;
  }
  if ((attributes & vertex_attribute_type_texcoord) != 0) {
    utstring_printf(s, "layout(location = %u) in vec2 inTexCoord;\n", location);
    location++;
  }
  if ((attributes & vertex_attribute_type_tangent) != 0) {
    utstring_printf(s, "layout(location = %u) in vec4 inTangent;\n", location);
  }
}

void glsl_add_vertex_shader_output_variables(UT_string *s) {
  uint32_t location = 0;
  utstring_printf(s, "layout(location = %u) out flat uint outInstanceId;\n", location);
  location++;
  utstring_printf(s, "layout(location = %u) out vec3 outPosition;\n", location);
  location++;
  utstring_printf(s, "#ifdef IN_COLOR\n");
  utstring_printf(s, "layout(location = %u) out vec3 outColor;\n", location);
  utstring_printf(s, "#endif\n");
  location++;
  utstring_printf(s, "#ifdef IN_NORMAL\n");
  utstring_printf(s, "layout(location = %u) out vec3 outNormal;\n", location);
  utstring_printf(s, "#endif\n");
  location++;
  utstring_printf(s, "#ifdef IN_TEXCOORD\n");
  utstring_printf(s, "layout(location = %u) out vec2 outTexCoord;\n", location);
  utstring_printf(s, "#endif\n");
  location++;
  utstring_printf(s, "#ifdef IN_TANGENT\n");
  utstring_printf(s, "layout(location = %u) out vec3 outTangent;\n", location);
  location++;
  utstring_printf(s, "layout(location = %u) out vec3 outBitangent;\n", location);
  utstring_printf(s, "#endif\n");
}

void glsl_add_fragment_shader_early_fragment_tests(UT_string *s, bool forceEarlyFragmentTests) {
  if (forceEarlyFragmentTests) {
    utstring_printf(s, "layout(early_fragment_tests) in;\n");
  }
}

void glsl_add_fragment_shader_input_variables(UT_string *s) {
  uint32_t location = 0;
  utstring_printf(s, "layout(location = %u) in flat uint inInstanceId;\n", location);
  location++;
  utstring_printf(s, "layout(location = %u) in vec3 inPosition;\n", location);
  location++;
  utstring_printf(s, "#ifdef IN_COLOR\n");
  utstring_printf(s, "layout(location = %u) in vec3 inColor;\n", location);
  utstring_printf(s, "#endif\n");
  location++;
  utstring_printf(s, "#ifdef IN_NORMAL\n");
  utstring_printf(s, "layout(location = %u) in vec3 inNormal;\n", location);
  utstring_printf(s, "#endif\n");
  location++;
  utstring_printf(s, "#ifdef IN_TEXCOORD\n");
  utstring_printf(s, "layout(location = %u) in vec2 inTexCoord;\n", location);
  utstring_printf(s, "#endif\n");
  location++;
  utstring_printf(s, "#ifdef IN_TANGENT\n");
  utstring_printf(s, "layout(location = %u) in vec3 inTangent;\n", location);
  location++;
  utstring_printf(s, "layout(location = %u) in vec3 inBitangent;\n", location);
  utstring_printf(s, "#endif\n");
}

void glsl_add_fragment_shader_output_variables(UT_string *s,
                                               uint32_t framebufferColorAttachmentCount) {
  for (uint32_t i = 0; i < framebufferColorAttachmentCount; i++) {
    utstring_printf(s, "layout(location = %u) out vec4 outFragColor%u;\n", i, i);
  }
}

void glsl_add_descriptors(UT_string *s, descriptors *descriptors) {

#define x(_name, ...) glsl_add_##_name##_helper_element(s);
  VULKAN_HELPER_STRUCTS(x, )
#undef x

#define str_push_constant(_name, ...) glsl_add_##_name##_push_constant(s);
  VULKAN_PUSH_CONSTANTS(str_push_constant, )
#undef str_push_constant

#define str_uniform_buffer(_name, ...)                                                             \
  glsl_add_##_name##_uniform_buffer(                                                               \
      s, descriptors->descriptorSetNumber,                                                         \
      descriptors->_name##UniformBufferBinding->bindingNumber,                                     \
      _name##_uniform_buffer_data_get_count(descriptors->unifiedUniformBuffer->_name##Data));
  VULKAN_UNIFORM_BUFFERS(str_uniform_buffer, )
#undef str_uniform_buffer

  glsl_add_textures(s, descriptors->descriptorSetNumber, descriptors->texturesBinding.bindingNumber,
                    descriptors->texturesBinding.textures);
}

void glsl_add_entry_point_begin(UT_string *s) { utstring_printf(s, "%s\n", "void main() {\n"); }

void glsl_add_entry_point_end(UT_string *s) { utstring_printf(s, "%s\n", "}"); }

void glsl_add_offscreen_texture_idxes(UT_string *s, render_pass_desc renderPassDesc) {
  for (size_t i = 0; i < renderPassDesc.offscreenFragmentShaderInputCount; i++) {
    assert(renderPassDesc.offscreenFragmentShaderInputs[i]._offscreenTextureIdx !=
           MAX_OFFSCREEN_TEXTURE_COUNT);
    utstring_printf(s, "uint %sOffscreenTextureIdx = %u;\n",
                    renderPassDesc.offscreenFragmentShaderInputs[i].name,
                    renderPassDesc.offscreenFragmentShaderInputs[i]._offscreenTextureIdx);
  }
}

void glsl_add_body(UT_string *s, render_pass_desc renderPassDesc, shader_type shaderType) {
  const char *filename = NULL;
  if (shaderType == shader_type_vertex) {
    filename = renderPassDesc.vertexShader;
  } else if (shaderType == shader_type_fragment) {
    filename = renderPassDesc.fragmentShader;
  } else {
    UNREACHABLE;
  }
  assert(filename != NULL);

  UT_string *inputPath = get_file_path(globals.assetsShaderDirpath, filename);
  UT_string *glslSource = read_text_file(inputPath);
  verify(utstring_len(glslSource) > 0);
  utstring_printf(s, "// %s\n%s\n", utstring_body(inputPath), utstring_body(glslSource));
  utstring_free(glslSource);
  utstring_free(inputPath);
}

shader *render_pass_shader_generator_get_shader(render_pass_shader_generator *shaderGenerator,
                                                render_pass_desc renderPassDesc,
                                                shader_type shaderType) {
  utstring_clear(shaderGenerator->sourceCode);

  glsl_add_header(shaderGenerator->sourceCode);
  glsl_add_defines(shaderGenerator->sourceCode, renderPassDesc, shaderType,
                   shaderGenerator->renderState);
  glsl_add_descriptors(shaderGenerator->sourceCode, shaderGenerator->renderState->descriptors);

  if (shaderType == shader_type_vertex) {
    glsl_add_vertex_shader_input_variables(shaderGenerator->sourceCode,
                                           shaderGenerator->renderState->vertexStream);
    glsl_add_vertex_shader_output_variables(shaderGenerator->sourceCode);
  } else if (shaderType == shader_type_fragment) {
    glsl_add_fragment_shader_early_fragment_tests(shaderGenerator->sourceCode,
                                                  renderPassDesc.forceEarlyFragmentTests);
    glsl_add_fragment_shader_input_variables(shaderGenerator->sourceCode);
    glsl_add_fragment_shader_output_variables(shaderGenerator->sourceCode,
                                              renderPassDesc.colorAttachmentCount);
  } else {
    UNREACHABLE;
  }

  glsl_add_common_source(shaderGenerator->sourceCode);

  glsl_add_entry_point_begin(shaderGenerator->sourceCode);
  glsl_add_offscreen_texture_idxes(shaderGenerator->sourceCode, renderPassDesc);
  glsl_add_body(shaderGenerator->sourceCode, renderPassDesc, shaderType);
  glsl_add_entry_point_end(shaderGenerator->sourceCode);

  return shader_create_with_str(shaderGenerator->renderState->vkd, shaderType,
                                shaderGenerator->sourceCode);
}
