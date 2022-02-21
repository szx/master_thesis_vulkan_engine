#include "shader.h"

static size_t vulkanShaderCount = 0;
static shaderc_compiler_t compiler;

static const char *shaderc_shader_kind_debug_str(shaderc_shader_kind type) {
  if (type == shaderc_vertex_shader) {
    return "vertex shader";
  }
  if (type == shaderc_fragment_shader) {
    return "fragment shader";
  }
  if (type == shaderc_compute_shader) {
    return "compute shader";
  }
  return "UNKNOWN shaderc_shader_kind";
}

void vulkan_vertex_attribute_description_init(vulkan_vertex_attribute_description *description,
                                              size_t location, size_t componentNum,
                                              const char *identifier, vulkan_attribute_type type) {
  description->location = location;
  description->componentNum = componentNum;
  description->identifier = strdup(identifier);
  description->type = type;
}

void vulkan_vertex_attribute_description_deinit(vulkan_vertex_attribute_description *description) {
  free(description->identifier);
}

vulkan_push_constant_description *vulkan_push_constant_description_create(const char *blockName,
                                                                          const char *instanceName,
                                                                          size_t size) {
  vulkan_push_constant_description *description = core_alloc(sizeof(*description));
  description->blockName = strdup(blockName);
  description->instanceName = strdup(instanceName);
  description->size = size;
  return description;
}

void vulkan_push_constant_description_destroy(vulkan_push_constant_description *description) {
  core_free(description->blockName);
  core_free(description->instanceName);
  core_free(description);
}


void vulkan_shader_info_init(vulkan_shader_info *info, vulkan_shader *shader) {
  log_debug("vulkan_shader_info_init");
  /*parser_state state = glsl_parser_execute(utstring_body(shader->glslCode));
  /// parser_debug_print(&state);
  verify(state.isValid == true);
  glsl_parser_callback_data data = {.state = &state, .pass = CountDescriptors, .info = info};
  parser_ast_node_visit(state.programNode, glsl_parser_callback, &data);
  core_array_alloc(info->inputAttributeDescriptions, data.inputAttributeDescriptionsSize);
  core_array_alloc(info->outputAttributeDescriptions, data.outputAttributeDescriptionsSize);
  info->pushConstantDescription = NULL;
  data.pass = ParseDescriptors;
  parser_ast_node_visit(state.programNode, glsl_parser_callback, &data);
  data.pass = Finished;
  parser_state_free(&state);*/
}

void vulkan_shader_info_deinit(vulkan_shader_info *info) {
  core_array_foreach(info->inputAttributeDescriptions,
                     vulkan_vertex_attribute_description * description,
                     { vulkan_vertex_attribute_description_deinit(description); });
  core_array_dealloc(info->inputAttributeDescriptions);
  core_array_foreach(info->outputAttributeDescriptions,
                     vulkan_vertex_attribute_description * description,
                     { vulkan_vertex_attribute_description_deinit(description); });
  core_array_dealloc(info->outputAttributeDescriptions);
  vulkan_push_constant_description_destroy(info->pushConstantDescription);
}

size_t vulkan_shader_info_get_binding_count(vulkan_shader_info *info) {
  // TODO: More bindings if we decide to bind more buffers in vkCmdBindVertexBuffers(). (instancing)
  return 1;
}

VkVertexInputBindingDescription
vulkan_shader_info_get_binding_description(vulkan_shader_info *info) {
  VkVertexInputBindingDescription bindingDescription = {0};
  bindingDescription.binding = 0;
  bindingDescription.stride = 0;
  for (size_t i = 0; i < core_array_count(info->inputAttributeDescriptions); i++) {
    vulkan_vertex_attribute_description *description = &info->inputAttributeDescriptions.ptr[i];
    // We do not support dvec.
    bindingDescription.stride += description->componentNum * sizeof(float);
  }
  assert(bindingDescription.stride > 0);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  // HIRO new binding with input rate instance for instance data
  return bindingDescription;
}

VkVertexInputAttributeDescription *
vulkan_shader_info_get_attribute_descriptions(vulkan_shader_info *info, size_t *count) {
  *count = core_array_count(info->inputAttributeDescriptions);
  VkVertexInputAttributeDescription *attributeDescriptions =
      (VkVertexInputAttributeDescription *)malloc(*count *
                                                  sizeof(VkVertexInputAttributeDescription));
  uint32_t offset = 0;
  for (size_t i = 0; i < *count; i++) {
    vulkan_vertex_attribute_description *description = &info->inputAttributeDescriptions.ptr[i];
    VkFormat format = VK_FORMAT_R32_SFLOAT + 3 * (description->componentNum - 1);
    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = description->location;
    attributeDescriptions[i].format = format;
    attributeDescriptions[i].offset = offset;
    offset += description->componentNum * sizeof(float);
  }
  assert(offset > 0);
  return attributeDescriptions;
}

VkPushConstantRange vulkan_shader_info_get_push_constant_range(vulkan_shader *vertShader,
                                                               vulkan_shader *fragShader) {
  VkPushConstantRange pushConstantRange = {0};
  vulkan_push_constant_description *vertPc = vertShader->info.pushConstantDescription;
  vulkan_push_constant_description *fragPc = fragShader->info.pushConstantDescription;
  pushConstantRange.stageFlags =
      vulkan_shader_info_get_push_constant_stage_flags(vertShader, fragShader);
  pushConstantRange.offset = 0;
  verify(!(vertPc && fragPc) || vertPc->size == fragPc->size);
  pushConstantRange.size = vertPc->size;
  return pushConstantRange;
}

VkShaderStageFlagBits vulkan_shader_info_get_push_constant_stage_flags(vulkan_shader *vertShader,
                                                                       vulkan_shader *fragShader) {
  vulkan_push_constant_description *vertPc = vertShader->info.pushConstantDescription;
  vulkan_push_constant_description *fragPc = fragShader->info.pushConstantDescription;
  VkShaderStageFlagBits stageFlags = 0;
  if (vertPc != NULL) {
    stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
  }
  if (fragPc != NULL) {
    stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
  }
  return stageFlags;
}

vulkan_shader *vulkan_shader_create_with_path(vulkan_device *vkd, UT_string *glslPath) {
  UT_string *glslCode = read_text_file(glslPath);
  verify(utstring_len(glslCode) > 0);
  shaderc_shader_kind type = shaderc_glsl_vertex_shader;
  if (path_ext_equal(glslPath, ".vert")) {
    type = shaderc_glsl_vertex_shader;
  } else if (path_ext_equal(glslPath, ".frag")) {
    type = shaderc_glsl_fragment_shader;
  } else {
    panic("unknown glsl shader extension");
  }
  vulkan_shader *shader = vulkan_shader_create_with_str(vkd, type, glslCode);
  utstring_free(glslCode);
  return shader;
}

vulkan_shader *vulkan_shader_create_with_str(vulkan_device *vkd, shaderc_shader_kind type,
                                             UT_string *text) {
  vulkan_shader *shader = core_alloc(sizeof(vulkan_shader));
  shader->vkd = vkd;
  utstring_new(shader->glslCode);
  utstring_concat(shader->glslCode, text);
  shader->type = type;
  verify(utstring_len(shader->glslCode) > 0);
  if (vulkanShaderCount == 0) {
    compiler = shaderc_compiler_initialize();
  }
  verify(compiler != NULL);
  vulkanShaderCount += 1;
  shaderc_compile_options_t options = shaderc_compile_options_initialize();
  shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan, 0);
  shaderc_compilation_result_t result = shaderc_compile_into_spv(
      compiler, utstring_body(shader->glslCode), utstring_len(shader->glslCode), shader->type,
      "shader", "main", NULL);
  shaderc_compile_options_release(options);
  if (shaderc_result_get_num_errors(result)) {
    panic("compilation error: %s\n", shaderc_result_get_error_message(result));
  }
  shader->spvSize = shaderc_result_get_length(result);
  shader->spvCode = (uint32_t *)malloc(shader->spvSize);
  core_memcpy(shader->spvCode, (uint32_t *)shaderc_result_get_bytes(result), shader->spvSize);
  shaderc_result_release(result);
  shader->module = vulkan_create_shader_module(shader->vkd, shader->spvCode, shader->spvSize,
                                               shaderc_shader_kind_debug_str(shader->type));
  vulkan_shader_info_init(&shader->info, shader);
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
  vulkan_shader_info_deinit(&shader->info);
  core_free(shader);
}
