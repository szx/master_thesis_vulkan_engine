#include "shader.h"
#include "../codegen/shader.c"

#include "../peg/glsl_parser.h"

static size_t vulkanShaderCount = 0;
static shaderc_compiler_t compiler;

void vulkan_vertex_attribute_description_init(vulkan_vertex_attribute_description *description,
                                              size_t location, size_t componentNum,
                                              const char *identifier) {
  description->location = location;
  description->componentNum = componentNum;
  description->identifier = strdup(identifier);
}

void vulkan_vertex_attribute_description_deinit(vulkan_vertex_attribute_description *description) {
  free(description->identifier);
}

void vulkan_push_constant_description_init(vulkan_push_constant_description *description,
                                           const char *blockName, const char *instanceName,
                                           size_t size) {
  description->blockName = strdup(blockName);
  description->instanceName = strdup(instanceName);
  description->size = size;
}

void vulkan_push_constant_description_deinit(vulkan_push_constant_description *description) {
  free(description->blockName);
  free(description->instanceName);
}

typedef enum glsl_parser_callback_pass {
  CountDescriptors,
  ParseDescriptors,
  Finished
} glsl_parser_callback_pass;

typedef struct glsl_parser_callback_data {
  parser_state *state;
  glsl_parser_callback_pass pass;
  size_t inputAttributeDescriptionsSize;
  size_t inputAttributeDescriptionsIdx;
  size_t outputAttributeDescriptionsSize;
  size_t outputAttributeDescriptionsIdx;
  vulkan_shader_info *info;
} glsl_parser_callback_data;

static size_t glsl_parser_node_to_size(parser_state *state, parser_ast_node *node) {
  if (node->type == VariableDeclaration) {
    parser_ast_node *typeNode = node->childNodes;
    parser_ast_node *componentNode = typeNode->childNodes;
    size_t componentNum = parser_ast_node_convert_int(state, componentNode);
    if (typeNode->type == VectorType) {
      return componentNum * sizeof(float);
    }
    if (typeNode->type == MatrixType) {
      return componentNum * componentNum * sizeof(float);
    }
  }
  assert(0);
  return 0;
}

static bool glsl_parser_callback(parser_ast_node *node, void *callbackData) {
  glsl_parser_callback_data *data = callbackData;
  if (data->pass == CountDescriptors) {
    if (node->type == VertexInputAttribute) {
      data->inputAttributeDescriptionsSize += 1;
    } else if (node->type == VertexOutputAttribute) {
      data->outputAttributeDescriptionsSize += 1;
    }
  } else if (data->pass == ParseDescriptors) {
    if (node->type == VertexInputAttribute || node->type == VertexOutputAttribute) {
      parser_ast_node *locationNode = node->childNodes;
      size_t location = parser_ast_node_convert_int(data->state, locationNode);

      parser_ast_node *vectorNode = locationNode->next;
      size_t componentNum = parser_ast_node_convert_int(data->state, vectorNode->childNodes);

      parser_ast_node *identifierNode = vectorNode->next;
      char *identifier = parser_ast_node_c_str(data->state, identifierNode);

      if (node->type == VertexInputAttribute) {
        init_struct_array_elem(
            data->info->inputAttributeDescriptions, data->inputAttributeDescriptionsIdx,
            vulkan_vertex_attribute_description_init, location, componentNum, identifier);
        data->inputAttributeDescriptionsIdx += 1;
        verify(data->inputAttributeDescriptionsIdx <= data->inputAttributeDescriptionsSize);
      } else if (node->type == VertexOutputAttribute) {
        init_struct_array_elem(
            data->info->outputAttributeDescriptions, data->outputAttributeDescriptionsIdx,
            vulkan_vertex_attribute_description_init, location, componentNum, identifier);
        data->outputAttributeDescriptionsIdx += 1;
        verify(data->outputAttributeDescriptionsIdx <= data->outputAttributeDescriptionsSize);
      }
      free(identifier);
    } else if (node->type == PushConstant) {
      char *blockName = parser_ast_node_c_str(data->state, node->childNodes);
      char *instanceName = parser_ast_node_c_str(data->state, node->childNodes->next);
      parser_ast_node *uniformBlockNode = node->childNodes->next->next;
      size_t size = 0;
      parser_ast_node *variableDeclaration;
      LL_FOREACH(uniformBlockNode->childNodes, variableDeclaration) {
        size += glsl_parser_node_to_size(data->state, variableDeclaration);
      }
      data->info->pushConstantDescription = alloc_struct(vulkan_push_constant_description);
      init_struct(data->info->pushConstantDescription, vulkan_push_constant_description_init,
                  blockName, instanceName, size);
      free(blockName);
      free(instanceName);
    }
  }
  return node->type == TranslationUnit;
}

void vulkan_shader_info_init(vulkan_shader_info *info, vulkan_shader *shader) {
  log_debug("vulkan_shader_info_init");
  parser_state state = glsl_parser_execute(str_c_str(&shader->glslCode));
  parser_debug_print(&state);
  verify(state.isValid == true);
  glsl_parser_callback_data data = {.state = &state, .pass = CountDescriptors, .info = info};
  parser_ast_node_visit(state.programNode, glsl_parser_callback, &data);
  info->inputAttributeDescriptions =
      alloc_struct_array(vulkan_vertex_attribute_description, data.inputAttributeDescriptionsSize);
  info->outputAttributeDescriptions =
      alloc_struct_array(vulkan_vertex_attribute_description, data.outputAttributeDescriptionsSize);
  info->pushConstantDescription = NULL;
  data.pass = ParseDescriptors;
  parser_ast_node_visit(state.programNode, glsl_parser_callback, &data);
  data.pass = Finished;
  parser_state_free(&state);
}

void vulkan_shader_info_deinit(vulkan_shader_info *info) {
  dealloc_struct(info->inputAttributeDescriptions);
  dealloc_struct(info->outputAttributeDescriptions);
  dealloc_struct(info->pushConstantDescription);
}

VkVertexInputBindingDescription
vulkan_shader_info_get_binding_description(vulkan_shader_info *info) {
  // TODO: More bindings if we decide to bind more buffers in vkCmdBindVertexBuffers(). (instancing)
  VkVertexInputBindingDescription bindingDescription = {0};
  bindingDescription.binding = 0;
  bindingDescription.stride = 0;
  for (size_t i = 0; i < count_struct_array(info->inputAttributeDescriptions); i++) {
    vulkan_vertex_attribute_description *description = &info->inputAttributeDescriptions[i];
    // We do not support dvec.
    bindingDescription.stride += description->componentNum * sizeof(float);
  }
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescription;
}

VkVertexInputAttributeDescription *
vulkan_shader_info_get_attribute_descriptions(vulkan_shader_info *info, size_t *count) {
  *count = count_struct_array(info->inputAttributeDescriptions);
  VkVertexInputAttributeDescription *attributeDescriptions =
      (VkVertexInputAttributeDescription *)malloc(*count *
                                                  sizeof(VkVertexInputAttributeDescription));
  uint32_t offset = 0;
  for (size_t i = 0; i < *count; i++) {
    vulkan_vertex_attribute_description *description = &info->inputAttributeDescriptions[i];
    VkFormat format = VK_FORMAT_R32_SFLOAT + 3 * (description->componentNum - 1);
    attributeDescriptions[i].binding = 0;
    attributeDescriptions[i].location = description->location;
    attributeDescriptions[i].format = format;
    attributeDescriptions[i].offset = offset;
    offset += description->componentNum * sizeof(float);
  }
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

void vulkan_shader_init_with_path(vulkan_shader *shader, vulkan_device *vkd,
                                  platform_path glslPath) {
  char *input = read_text_file(&glslPath, NULL);
  str glslCode = str_init(input);
  free(input);
  verify(glslCode.size > 0);
  shaderc_shader_kind type = shaderc_glsl_vertex_shader;
  if (platform_path_ext_equals(&glslPath, ".vert")) {
    type = shaderc_glsl_vertex_shader;
  } else if (platform_path_ext_equals(&glslPath, ".frag")) {
    type = shaderc_glsl_fragment_shader;
  } else {
    panic("unknown glsl shader extension");
  }
  vulkan_shader_init_with_str(shader, vkd, type, &glslCode);
  str_free(&glslCode);
}

void vulkan_shader_init_with_str(vulkan_shader *shader, vulkan_device *vkd,
                                 shaderc_shader_kind type, str *text) {
  shader->vkd = vkd;
  shader->glslCode = str_copy(text);
  shader->type = type;
  verify(shader->glslCode.size > 0);
  if (vulkanShaderCount == 0) {
    compiler = shaderc_compiler_initialize();
  }
  verify(compiler != NULL);
  vulkanShaderCount += 1;
  shaderc_compile_options_t options = shaderc_compile_options_initialize();
  shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan, 0);
  shaderc_compilation_result_t result =
      shaderc_compile_into_spv(compiler, str_c_str(&shader->glslCode), shader->glslCode.size,
                               shader->type, "shader", "main", NULL);
  shaderc_compile_options_release(options);
  if (shaderc_result_get_num_errors(result)) {
    panic("compilation error: %s\n", shaderc_result_get_error_message(result));
  }
  shader->spvSize = shaderc_result_get_length(result);
  shader->spvCode = (uint32_t *)malloc(shader->spvSize);
  memcpy(shader->spvCode, (uint32_t *)shaderc_result_get_bytes(result), shader->spvSize);
  shaderc_result_release(result);
  shader->module = create_shader_module(shader->vkd, shader->spvCode, shader->spvSize);
  vulkan_shader_info_init(&shader->info, shader);
}

void vulkan_shader_deinit(vulkan_shader *shader) {
  vulkanShaderCount -= 1;
  if (vulkanShaderCount == 0) {
    shaderc_compiler_release(compiler);
  }
  str_free(&shader->glslCode);
  free(shader->spvCode);
  vkDestroyShaderModule(shader->vkd->device, shader->module, vka);
  vulkan_shader_info_deinit(&shader->info);
}
