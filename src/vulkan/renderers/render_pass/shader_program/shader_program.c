#include "shader_program.h"

vulkan_render_pass_shader_program *
vulkan_render_pass_shader_program_create(vulkan_render_state *renderState,
                                         vulkan_render_pass_desc renderPassDesc) {
  vulkan_render_pass_shader_program *shaderProgram =
      core_alloc(sizeof(vulkan_render_pass_shader_program));

  vulkan_render_pass_shader_generator_init(&shaderProgram->shaderGenerator, renderState);

  shaderProgram->vertexShader = vulkan_render_pass_shader_generator_get_shader(
      &shaderProgram->shaderGenerator, renderPassDesc, vulkan_shader_type_vertex);
  shaderProgram->fragmentShader = vulkan_render_pass_shader_generator_get_shader(
      &shaderProgram->shaderGenerator, renderPassDesc, vulkan_shader_type_fragment);

  return shaderProgram;
}

void vulkan_render_pass_shader_program_destroy(vulkan_render_pass_shader_program *shaderProgram) {
  vulkan_shader_destroy(shaderProgram->vertexShader);
  vulkan_shader_destroy(shaderProgram->fragmentShader);
  vulkan_render_pass_shader_generator_deinit(&shaderProgram->shaderGenerator);
  core_free(shaderProgram);
}

VkPipelineShaderStageCreateInfo *vulkan_render_pass_shader_program_get_shader_stages(
    vulkan_render_pass_shader_program *shaderProgram, size_t *count) {
  size_t shaderStageCount = 2;
  *count = shaderStageCount;

  VkPipelineShaderStageCreateInfo *shaderStages =
      core_alloc(shaderStageCount * sizeof(VkPipelineShaderStageCreateInfo));

  VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {0};
  vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexShaderStageInfo.module = shaderProgram->vertexShader->module;
  vertexShaderStageInfo.pName = "main";
  shaderStages[0] = vertexShaderStageInfo;

  VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {0};
  fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStageInfo.module = shaderProgram->fragmentShader->module;
  fragmentShaderStageInfo.pName = "main";
  shaderStages[1] = fragmentShaderStageInfo;

  return shaderStages;
}

void vulkan_render_pass_shader_program_debug_print(vulkan_render_pass_shader_program *shaderProgram,
                                                   int indent) {
  log_debug(INDENT_FORMAT_STRING "shader generator:", INDENT_FORMAT_ARGS(0));
  vulkan_shader_debug_print(shaderProgram->vertexShader, indent + 2);
  vulkan_shader_debug_print(shaderProgram->fragmentShader, indent + 2);
}
