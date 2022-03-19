#include "shader_generator.h"

void glsl_add_header(UT_string *s) { utstring_printf(s, "%s\n", "#version 450"); }

void glsl_add_vertex_shader_input_variables_defines(UT_string *s,
                                                    vulkan_interleaved_vertex_stream *stream) {
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
  glsl_add_vertex_shader_input_variables_defines(s, renderState->vertexStream);
}

void glsl_add_vertex_shader_input_variables(UT_string *s,
                                            vulkan_interleaved_vertex_stream *stream) {
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

void glsl_add_uniform_buffers(UT_string *s, vulkan_unified_uniform_buffer *unifiedUniformBuffer) {
  vulkan_descriptors *descriptors = unifiedUniformBuffer->descriptors;
  uint32_t set = 0;
  uint32_t binding = 0;
#define str_uniform_buffer(_name, ...)                                                             \
  glsl_add_vulkan_##_name##_uniform_buffer(s, descriptors->_name##DescriptorSetNum,                \
                                           descriptors->_name##DescriptorBindingNum,               \
                                           descriptors->unifiedUniformBuffer->_name##Data->count);
  VULKAN_UNIFORM_BUFFERS(str_uniform_buffer, )
#undef str_uniform_buffer
}

void glsl_add_entry_point_begin(UT_string *s) { utstring_printf(s, "%s\n", "void main() {\n"); }

void glsl_add_entry_point_end(UT_string *s) { utstring_printf(s, "%s\n", "}"); }

void glsl_add_body(UT_string *s, vulkan_shader_type type) {
  const char *filename = "invalid shader name";
  if (type == vulkan_shader_type_vertex) {
    filename = "vertex.glsl";
  } else if (type == vulkan_shader_type_fragment) {
    filename = "fragment.glsl";
  } else {
    assert(0);
  }
  UT_string *inputPath = get_asset_file_path("shaders", filename);
  UT_string *glslSource = read_text_file(inputPath);
  utstring_printf(s, "// %s\n%s\n", utstring_body(inputPath), utstring_body(glslSource));
  utstring_free(glslSource);
  utstring_free(inputPath);
}

vulkan_shader_generator *vulkan_shader_generator_create(vulkan_render_state *renderState) {
  vulkan_shader_generator *shaderGenerator = core_alloc(sizeof(vulkan_shader_generator));

  // Generate shaders.
  UT_string *s;

  utstring_new(s);
  glsl_add_header(s);
  glsl_add_defines(s, renderState);
  glsl_add_vertex_shader_input_variables(s, renderState->vertexStream);
  glsl_add_vertex_shader_output_variables(s);
  glsl_add_uniform_buffers(s, renderState->unifiedUniformBuffer);
  glsl_add_entry_point_begin(s);
  glsl_add_body(s, vulkan_shader_type_vertex);
  glsl_add_entry_point_end(s);
  shaderGenerator->vertexShader =
      vulkan_shader_create_with_str(renderState->vkd, vulkan_shader_type_vertex, s);

  utstring_clear(s);
  glsl_add_header(s);
  glsl_add_defines(s, renderState);
  glsl_add_fragment_shader_input_variables(s);
  glsl_add_fragment_shader_output_variables(s);
  glsl_add_uniform_buffers(s, renderState->unifiedUniformBuffer);
  glsl_add_entry_point_begin(s);
  glsl_add_body(s, vulkan_shader_type_fragment);
  glsl_add_entry_point_end(s);
  shaderGenerator->fragmentShader =
      vulkan_shader_create_with_str(renderState->vkd, vulkan_shader_type_fragment, s);

  utstring_free(s);

  // TODO: Deferred renderer.
  return shaderGenerator;
}

void vulkan_shader_generator_destroy(vulkan_shader_generator *shaderGenerator) {
  vulkan_shader_destroy(shaderGenerator->vertexShader);
  vulkan_shader_destroy(shaderGenerator->fragmentShader);
  core_free(shaderGenerator);
}

VkPipelineShaderStageCreateInfo *
vulkan_shader_generator_get_shader_stages(vulkan_shader_generator *shaderGenerator, size_t *count) {
  size_t shaderStageCount = 2;
  *count = shaderStageCount;

  VkPipelineShaderStageCreateInfo *shaderStages =
      core_alloc(shaderStageCount * sizeof(VkPipelineShaderStageCreateInfo));

  VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {0};
  vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexShaderStageInfo.module = shaderGenerator->vertexShader->module;
  vertexShaderStageInfo.pName = "main";
  shaderStages[0] = vertexShaderStageInfo;

  VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {0};
  fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStageInfo.module = shaderGenerator->fragmentShader->module;
  fragmentShaderStageInfo.pName = "main";
  shaderStages[1] = fragmentShaderStageInfo;

  return shaderStages;
}

void vulkan_shader_generator_debug_print(vulkan_shader_generator *shaderGenerator, int indent) {
  log_debug(INDENT_FORMAT_STRING "shader generator:", INDENT_FORMAT_ARGS(0));
  vulkan_shader_debug_print(shaderGenerator->vertexShader, indent + 2);
  vulkan_shader_debug_print(shaderGenerator->fragmentShader, indent + 2);
}
