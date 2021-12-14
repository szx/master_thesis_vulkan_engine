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
      lst_parser_ast_node_ptr_it it = lst_parser_ast_node_ptr_it_each(&node->childNodes);

      parser_ast_node *locationNode = it.ref->node;
      size_t location = parser_ast_node_convert_int(data->state, locationNode);
      lst_parser_ast_node_ptr_it_step(&it);

      parser_ast_node *vectorNode = it.ref->node->childNodes.head->value.node;
      size_t componentNum = parser_ast_node_convert_int(data->state, vectorNode);
      lst_parser_ast_node_ptr_it_step(&it);

      parser_ast_node *identifierNode = it.ref->node;
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
    }
  }
  return node->type == TranslationUnit;
}

void vulkan_shader_info_init(vulkan_shader_info *info, vulkan_shader *shader) {
  log_debug("vulkan_shader_info_init");
  parser_state state = glsl_parser_execute(shader->glslCode);
  verify(state.isValid == true);
  glsl_parser_callback_data data = {.state = &state, .pass = CountDescriptors, .info = info};
  parser_ast_node_visit(state.programNode, glsl_parser_callback, &data);
  info->inputAttributeDescriptions =
      alloc_struct_array(vulkan_vertex_attribute_description, data.inputAttributeDescriptionsSize);
  info->outputAttributeDescriptions =
      alloc_struct_array(vulkan_vertex_attribute_description, data.outputAttributeDescriptionsSize);
  data.pass = ParseDescriptors;
  parser_ast_node_visit(state.programNode, glsl_parser_callback, &data);
  data.pass = Finished;
  parser_state_free(&state);
}

void vulkan_shader_info_deinit(vulkan_shader_info *info) {
  dealloc_struct(info->inputAttributeDescriptions);
  dealloc_struct(info->outputAttributeDescriptions);
}

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
  vulkan_shader_info_init(&shader->info, shader);
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
  vulkan_shader_info_deinit(&shader->info);
}
