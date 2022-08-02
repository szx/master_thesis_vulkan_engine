#include "shader.h"
#include "shader_reflect.h"

static size_t vulkanShaderCount = 0;
static shaderc_compiler_t compiler;

shaderc_shader_kind shader_type_shaderc_kind(shader_type type) {
  if (type == shader_type_vertex) {
    return shaderc_glsl_vertex_shader;
  }
  if (type == shader_type_fragment) {
    return shaderc_glsl_fragment_shader;
  }
  UNREACHABLE;
}

VkShaderStageFlags shader_type_to_stage_flag(shader_type type) {
  if (type == shader_type_vertex) {
    return VK_SHADER_STAGE_VERTEX_BIT;
  }
  if (type == shader_type_fragment) {
    return VK_SHADER_STAGE_FRAGMENT_BIT;
  }
  UNREACHABLE;
}

shader *shader_create_with_str(device *vkd, shader_type type, UT_string *text) {
  shader *shader = core_alloc(sizeof(struct shader));

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
  const char *inputFileName = "shader";
  const char *entryPointName = "main";
  shaderc_compilation_result_t result = shaderc_compile_into_spv(
      compiler, utstring_body(shader->glslCode), utstring_len(shader->glslCode),
      shader_type_shaderc_kind(shader->type), inputFileName, entryPointName, NULL);
  shaderc_compile_options_release(options);
  if (shaderc_result_get_num_errors(result)) {
    // log GLSL source code with line numbers and compilation error message.
    const char *errorMsg = shaderc_result_get_error_message(result);
    const char *lineMsg = utstring_body(shader->glslCode);
    size_t lineMsgIdx = 0;
    size_t lineNum = 1;
    while (lineMsgIdx < utstring_len(shader->glslCode)) {
      const char *newLineMsg = strchr(lineMsg, '\n') + 1;
      size_t lineMsgLen = newLineMsg - lineMsg;
      log_fatal("%d:\t%.*s", lineNum++, lineMsgLen - 1, lineMsg);
      lineMsg = newLineMsg;
      lineMsgIdx += lineMsgLen;
    }
    panic("compilation error: %s\n", errorMsg);
  }
  shader->spvSize = shaderc_result_get_length(result);
  shader->spvCode = (uint32_t *)malloc(shader->spvSize);
  core_memcpy(shader->spvCode, (uint32_t *)shaderc_result_get_bytes(result), shader->spvSize);
  shaderc_result_release(result);
  shader->module = create_shader_module(shader->vkd, shader->spvCode, shader->spvSize,
                                        shader_type_debug_str(shader->type));

  shader->reflect = shader_reflect_create(shader->spvCode, shader->spvSize);

  shader_debug_print(shader, 0);

  return shader;
}

void shader_destroy(shader *shader) {
  vulkanShaderCount -= 1;
  if (vulkanShaderCount == 0) {
    shaderc_compiler_release(compiler);
  }
  utstring_free(shader->glslCode);
  free(shader->spvCode);
  vkDestroyShaderModule(shader->vkd->device, shader->module, vka);

  shader_reflect_destroy(shader->reflect);

  core_free(shader);
}

void shader_debug_print(shader *shader, int indent) {
  log_debug(INDENT_FORMAT_STRING "shader:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "type: %s", INDENT_FORMAT_ARGS(2),
            shader_type_debug_str(shader->type));
  log_debug(INDENT_FORMAT_STRING "spvSize %zu:", INDENT_FORMAT_ARGS(2), shader->spvSize);
  log_debug(INDENT_FORMAT_STRING "glslCode: %s", INDENT_FORMAT_ARGS(2),
            utstring_body(shader->glslCode));
  // shader_reflect_debug_print(shader->reflect, 2);
}
