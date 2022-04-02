#include "shader_generator.h"

void vulkan_shader_generator_init(vulkan_shader_generator *shaderGenerator,
                                  vulkan_render_state *renderState) {
  shaderGenerator->renderState = renderState;
  utstring_new(shaderGenerator->sourceCode);
}

void vulkan_shader_generator_deinit(vulkan_shader_generator *shaderGenerator) {
  utstring_free(shaderGenerator->sourceCode);
}

/* GLSL helper functions */

void glsl_add_header(UT_string *s) { utstring_printf(s, "%s\n", "#version 450"); }

void glsl_add_vertex_shader_input_variables_defines(UT_string *s, vulkan_vertex_stream *stream) {
  assert(stream->renderCacheList->attributes > 0);
  size_t attributes = stream->renderCacheList->attributes;

  if ((attributes & vulkan_attribute_type_position) != 0) {
    utstring_printf(s, "#define IN_POSITION\n");
  }
  if ((attributes & vulkan_attribute_type_normal) != 0) {
    utstring_printf(s, "#define IN_NORMAL\n");
  }
  if ((attributes & vulkan_attribute_type_color) != 0) {
    utstring_printf(s, "#define IN_COLOR\n");
  }
  if ((attributes & vulkan_attribute_type_texcoord) != 0) {
    utstring_printf(s, "#define IN_TEXCOORD\n");
  }
}

void glsl_add_defines(UT_string *s, vulkan_render_state *renderState) {
  glsl_add_vertex_shader_input_variables_defines(s,
                                                 renderState->unifiedGeometryBuffer->vertexStream);
}

void glsl_add_vertex_shader_input_variables(UT_string *s, vulkan_vertex_stream *stream) {
  assert(stream->renderCacheList->attributes > 0);
  size_t attributes = stream->renderCacheList->attributes;

  uint32_t location = 0;
  if ((attributes & vulkan_attribute_type_position) != 0) {
    utstring_printf(s, "layout(location = %u) in vec3 inPosition;\n", location);
  }
  location++;
  if ((attributes & vulkan_attribute_type_normal) != 0) {
    utstring_printf(s, "layout(location = %u) in vec3 inNormal;\n", location);
  }
  location++;
  if ((attributes & vulkan_attribute_type_color) != 0) {
    utstring_printf(s, "layout(location = %u) in vec3 inColor;\n", location);
  }
  location++;
  if ((attributes & vulkan_attribute_type_texcoord) != 0) {
    utstring_printf(s, "layout(location = %u) in vec3 inTexCoord;\n", location);
  }
}

void glsl_add_vertex_shader_output_variables(UT_string *s) {
  uint32_t location = 0;
  utstring_printf(s, "layout(location = %u) out vec3 outColor;\n", location);
  location++;
  utstring_printf(s, "#ifdef IN_TEXCOORD\n");
  utstring_printf(s, "layout(location = %u) out vec3 outTexCoord;\n", location);
  utstring_printf(s, "#endif\n");
}

void glsl_add_fragment_shader_input_variables(UT_string *s) {
  uint32_t location = 0;
  utstring_printf(s, "layout(location = %u) in vec3 inColor;\n", location);
  location++;
  utstring_printf(s, "#ifdef IN_TEXCOORD\n");
  utstring_printf(s, "layout(location = %u) in vec3 inTexCoord;\n", location);
  utstring_printf(s, "#endif\n");
}

void glsl_add_fragment_shader_output_variables(UT_string *s) {
  uint32_t location = 0;
  utstring_printf(s, "layout(location = %u) out vec4 outColor;\n", location);
}

void glsl_add_descriptors(UT_string *s, vulkan_descriptors *descriptors) {
  uint32_t set = 0;
  uint32_t binding = 0;
#define str_uniform_buffer(_name, ...)                                                             \
  glsl_add_vulkan_##_name##_uniform_buffer(s, descriptors->descriptorSetNumber,                    \
                                           descriptors->_name##DescriptorBinding->bindingNumber,   \
                                           descriptors->unifiedUniformBuffer->_name##Data->count);
  VULKAN_UNIFORM_BUFFERS(str_uniform_buffer, )
#undef str_uniform_buffer
  // HIRO HIRO glsl_add_vulkan_textures
}

void glsl_add_entry_point_begin(UT_string *s) { utstring_printf(s, "%s\n", "void main() {\n"); }

void glsl_add_entry_point_end(UT_string *s) { utstring_printf(s, "%s\n", "}"); }

void glsl_add_body(UT_string *s, vulkan_shader_generator_output_type type) {
  const char *filename = "invalid shader name";
  if (type == vulkan_shader_generator_output_type_forward_vertex) {
    filename = "forward_vertex.glsl";
  } else if (type == vulkan_shader_generator_output_type_forward_fragment) {
    filename = "forward_fragment.glsl";
  } else {
    assert(0);
  }
  UT_string *inputPath = get_asset_file_path("shaders", filename);
  UT_string *glslSource = read_text_file(inputPath);
  utstring_printf(s, "// %s\n%s\n", utstring_body(inputPath), utstring_body(glslSource));
  utstring_free(glslSource);
  utstring_free(inputPath);
}

vulkan_shader_type
vulkan_shader_generator_output_type_to_shader_type(vulkan_shader_generator_output_type type) {
  if (type == vulkan_shader_generator_output_type_forward_vertex) {
    return vulkan_shader_type_vertex;
  }
  if (type == vulkan_shader_generator_output_type_forward_fragment) {
    return vulkan_shader_type_fragment;
  }
  assert(0);
  return 0;
}

vulkan_shader *vulkan_shader_generator_get_shader(vulkan_shader_generator *shaderGenerator,
                                                  vulkan_shader_generator_output_type type) {
  utstring_clear(shaderGenerator->sourceCode);

  glsl_add_header(shaderGenerator->sourceCode);
  glsl_add_defines(shaderGenerator->sourceCode, shaderGenerator->renderState);
  glsl_add_descriptors(shaderGenerator->sourceCode, shaderGenerator->renderState->descriptors);

  vulkan_shader_type shaderType = vulkan_shader_generator_output_type_to_shader_type(type);
  if (shaderType == vulkan_shader_type_vertex) {
    glsl_add_vertex_shader_input_variables(
        shaderGenerator->sourceCode,
        shaderGenerator->renderState->unifiedGeometryBuffer->vertexStream);
    glsl_add_vertex_shader_output_variables(shaderGenerator->sourceCode);
  } else if (shaderType == vulkan_shader_type_fragment) {
    glsl_add_fragment_shader_input_variables(shaderGenerator->sourceCode);
    glsl_add_fragment_shader_output_variables(shaderGenerator->sourceCode);
  } else {
    assert(0);
    return NULL;
  }

  glsl_add_entry_point_begin(shaderGenerator->sourceCode);
  glsl_add_body(shaderGenerator->sourceCode, type);
  glsl_add_entry_point_end(shaderGenerator->sourceCode);

  return vulkan_shader_create_with_str(shaderGenerator->renderState->vkd, shaderType,
                                       shaderGenerator->sourceCode);
}
