#include "shader.h"
#include "../codegen/shader.c"

static size_t vulkanShaderCount = 0;
static shaderc_compiler_t compiler;

void vulkan_shader_init(vulkan_shader *shader, vulkan_device *vkd, platform_path glslPath) {
  shader->vkd = vkd;
  shader->glslPath = glslPath;
  shader->glslCode = read_text_file(&shader->glslPath, &shader->glslSize);
  verify(shader->glslCode != NULL);
  if (vulkanShaderCount == 0) {
    compiler = shaderc_compiler_initialize();
  }
  verify(compiler != NULL);
  vulkanShaderCount += 1;
  if (platform_path_ext_equals(&shader->glslPath, ".vert")) {
    shader->type = shaderc_glsl_vertex_shader;
  } else if (platform_path_ext_equals(&shader->glslPath, ".frag")) {
    shader->type = shaderc_glsl_fragment_shader;
  } else {
    panic("unknown glsl shader extension");
  }
  shaderc_compile_options_t options = shaderc_compile_options_initialize();
  shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan, 0);
  shaderc_compilation_result_t result = shaderc_compile_into_spv(
      compiler, shader->glslCode, shader->glslSize, shader->type, "main.vert", "main", NULL);
  shaderc_compile_options_release(options);
  if (shaderc_result_get_num_errors(result)) {
    panic("compilation error: %s\n", shaderc_result_get_error_message(result));
  }
  shader->spvSize = shaderc_result_get_length(result);
  shader->spvCode = (uint32_t *)malloc(shader->spvSize);
  memcpy(shader->spvCode, (uint32_t *)shaderc_result_get_bytes(result), shader->spvSize);
  shaderc_result_release(result);
  shader->module = create_shader_module(shader->vkd, shader->spvCode, shader->spvSize);
}

void vulkan_shader_deinit(vulkan_shader *shader) {
  vulkanShaderCount -= 1;
  if (vulkanShaderCount == 0) {
    shaderc_compiler_release(compiler);
  }
  platform_path_free(&shader->glslPath);
  free(shader->glslCode);
  free(shader->spvCode);
  vkDestroyShaderModule(shader->vkd->device, shader->module, vka);
}
