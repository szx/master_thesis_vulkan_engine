#include "shader_reflect.h"

vulkan_shader_reflect *vulkan_shader_reflect_create(uint32_t *spvCode, size_t spvSize) {
  vulkan_shader_reflect *reflect = core_alloc(sizeof(vulkan_shader_reflect));

  /* reflection */
  SpvReflectShaderModule reflectModule;
  verify(spvReflectCreateShaderModule(spvSize, spvCode, &reflectModule) ==
         SPV_REFLECT_RESULT_SUCCESS);

  // Input variables
  uint32_t inputVariableCount = 0;
  verify(spvReflectEnumerateInputVariables(&reflectModule, &inputVariableCount, NULL) ==
         SPV_REFLECT_RESULT_SUCCESS);
  SpvReflectInterfaceVariable **inputVariables = (SpvReflectInterfaceVariable **)malloc(
      inputVariableCount * sizeof(SpvReflectInterfaceVariable *));
  verify(spvReflectEnumerateInputVariables(&reflectModule, &inputVariableCount, inputVariables) ==
         SPV_REFLECT_RESULT_SUCCESS);

  utarray_alloc(reflect->inputVariables, sizeof(vulkan_shader_reflect_variable *));
  utarray_resize(reflect->inputVariables, inputVariableCount);
  for (size_t idx = 0; idx < inputVariableCount; idx++) {
    SpvReflectInterfaceVariable *spvReflect = inputVariables[idx];
    vulkan_shader_reflect_variable **variable = utarray_eltptr(reflect->inputVariables, idx);
    *variable = vulkan_shader_reflect_variable_create(spvReflect);
  }

  // HIRO: Output variables, descriptor bindings, descriptor sets, and push constants
  spvReflectDestroyShaderModule(&reflectModule);

  vulkan_shader_reflect_debug_print(reflect, 0);
  return reflect;
}

void vulkan_shader_reflect_destroy(vulkan_shader_reflect *reflect) {

  utarray_foreach_elem_deref (vulkan_shader_reflect_variable *, inputVariable,
                              reflect->inputVariables) {
    vulkan_shader_reflect_variable_destroy(inputVariable);
  }

  core_free(reflect);
}

void vulkan_shader_reflect_debug_print(vulkan_shader_reflect *reflect, int indent) {
  log_debug(INDENT_FORMAT_STRING "shader reflection:", INDENT_FORMAT_ARGS(0));
  utarray_foreach_elem_deref (vulkan_shader_reflect_variable *, inputVariable,
                              reflect->inputVariables) {
    vulkan_shader_reflect_variable_debug_print(inputVariable, 4);
  }
}

void copy_type_description(SpvReflectTypeDescription *dest, SpvReflectTypeDescription *src) {
  dest->id = src->id;
  dest->op = src->op;
  dest->type_name = core_strdup(src->type_name);
  dest->struct_member_name = core_strdup(src->struct_member_name);
  dest->storage_class = src->storage_class;
  dest->type_flags = src->type_flags;
  dest->decoration_flags = src->decoration_flags;
  dest->traits = src->traits;
  dest->member_count = 0;
  dest->members = NULL;
}

vulkan_shader_reflect_variable *
vulkan_shader_reflect_variable_create(SpvReflectInterfaceVariable *reflect) {
  vulkan_shader_reflect_variable *variable = core_alloc(sizeof(vulkan_shader_reflect_variable));
  variable->name = core_strdup(reflect->name);
  variable->location = reflect->location;
  variable->storageClass = reflect->storage_class;
  variable->builtIn = reflect->built_in;
  variable->numeric = reflect->numeric;
  variable->array = reflect->array;

  utarray_alloc(variable->members, sizeof(vulkan_shader_reflect_variable *));
  utarray_resize(variable->members, reflect->member_count);
  for (size_t idx = 0; idx < utarray_len(variable->members); idx++) {
    vulkan_shader_reflect_variable **member = utarray_eltptr(variable->members, idx);
    *member = vulkan_shader_reflect_variable_create(&reflect->members[idx]);
  }

  variable->format = reflect->format;

  copy_type_description(&variable->typeDescription, reflect->type_description);
  return variable;
}

void vulkan_shader_reflect_variable_destroy(vulkan_shader_reflect_variable *inputVariable) {
  core_free((char *)inputVariable->name);
  core_free(inputVariable);
}

void vulkan_shader_reflect_variable_debug_print(vulkan_shader_reflect_variable *inputVariable,
                                                int indent) {
  log_debug(INDENT_FORMAT_STRING "inputVariable:", INDENT_FORMAT_ARGS(2));
  log_debug(INDENT_FORMAT_STRING "name: %s", INDENT_FORMAT_ARGS(4), inputVariable->name);
  log_debug(INDENT_FORMAT_STRING "location: %d", INDENT_FORMAT_ARGS(4), inputVariable->location);
  log_debug(INDENT_FORMAT_STRING "storageClass: %s", INDENT_FORMAT_ARGS(4),
            SpvStorageClass__debug_str(inputVariable->storageClass));
  // HIRO more debug log, tests
  log_debug(INDENT_FORMAT_STRING "format: %s", INDENT_FORMAT_ARGS(4),
            SpvReflectFormat_debug_str(inputVariable->format));
  // log_debug(INDENT_FORMAT_STRING "inputVariable:", INDENT_FORMAT_ARGS(4)),
  // inputVariable->typeDescription.;
}
