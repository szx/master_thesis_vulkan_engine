#include "shader.h"

static size_t vulkanShaderCount = 0;
static shaderc_compiler_t compiler;

shaderc_shader_kind vulkan_shader_type_shaderc_kind(vulkan_shader_type type) {
  if (type == vulkan_shader_type_vertex) {
    return shaderc_glsl_vertex_shader;
  }
  if (type == vulkan_shader_type_fragment) {
    return shaderc_glsl_vertex_shader;
  }
  panic("unknown shader type");
  return shaderc_glsl_vertex_shader;
}

vulkan_shader *vulkan_shader_create_with_str(vulkan_device *vkd, vulkan_shader_type type,
                                             UT_string *text) {
  vulkan_shader *shader = core_alloc(sizeof(vulkan_shader));

  shader->vkd = vkd;
  utstring_new(shader->glslCode);
  utstring_concat(shader->glslCode, text);
  shader->type = type;
  verify(utstring_len(shader->glslCode) > 0);

  /* compiling */
  if (vulkanShaderCount == 0) {
    compiler = shaderc_compiler_initialize();
  }
  verify(compiler != NULL);
  vulkanShaderCount += 1;
  shaderc_compile_options_t options = shaderc_compile_options_initialize();
  shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan, 0);
  shaderc_compilation_result_t result = shaderc_compile_into_spv(
      compiler, utstring_body(shader->glslCode), utstring_len(shader->glslCode),
      vulkan_shader_type_shaderc_kind(shader->type), "shader", "main", NULL);
  shaderc_compile_options_release(options);
  if (shaderc_result_get_num_errors(result)) {
    panic("compilation error: %s\n", shaderc_result_get_error_message(result));
  }
  shader->spvSize = shaderc_result_get_length(result);
  shader->spvCode = (uint32_t *)malloc(shader->spvSize);
  core_memcpy(shader->spvCode, (uint32_t *)shaderc_result_get_bytes(result), shader->spvSize);
  shaderc_result_release(result);
  shader->module = vulkan_create_shader_module(shader->vkd, shader->spvCode, shader->spvSize,
                                               vulkan_shader_type_debug_str(shader->type));

  shader->reflect = vulkan_shader_reflect_create(shader->spvCode, shader->spvSize);

  vulkan_shader_debug_print(shader, 0);
  return shader;
}

void vulkan_shader_destroy(vulkan_shader *shader) {
  vulkanShaderCount -= 1;
  if (vulkanShaderCount == 0) {
    shaderc_compiler_release(compiler);
  }
  utstring_free(shader->glslCode);
  free(shader->spvCode);
  vkDestroyShaderModule(shader->vkd->device, shader->module, vka);

  vulkan_shader_reflect_destroy(shader->reflect);

  core_free(shader);
}

void vulkan_shader_debug_print(vulkan_shader *shader, int indent) {
  log_debug(INDENT_FORMAT_STRING "shader:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "type: %s", INDENT_FORMAT_ARGS(2),
            vulkan_shader_type_debug_str(shader->type));
  log_debug(INDENT_FORMAT_STRING "spvSize %zu:", INDENT_FORMAT_ARGS(2), shader->spvSize);
  vulkan_shader_reflect_debug_print(shader->reflect, 2);
}
