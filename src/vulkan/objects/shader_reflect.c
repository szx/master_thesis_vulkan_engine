#include "shader_reflect.h"

vulkan_shader_reflect *vulkan_shader_reflect_create(uint32_t *spvCode, size_t spvSize) {
  vulkan_shader_reflect *reflect = core_alloc(sizeof(vulkan_shader_reflect));

  /* reflection */
  SpvReflectShaderModule reflectModule;
  verify(spvReflectCreateShaderModule(spvSize, spvCode, &reflectModule) ==
         SPV_REFLECT_RESULT_SUCCESS);

  // input variables
  uint32_t inputVariableCount = 0;
  verify(spvReflectEnumerateInputVariables(&reflectModule, &inputVariableCount, NULL) ==
         SPV_REFLECT_RESULT_SUCCESS);
  SpvReflectInterfaceVariable **inputVariables = (SpvReflectInterfaceVariable **)core_alloc(
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
  core_free(inputVariables);

  // output variables
  uint32_t outputVariableCount = 0;
  verify(spvReflectEnumerateOutputVariables(&reflectModule, &outputVariableCount, NULL) ==
         SPV_REFLECT_RESULT_SUCCESS);
  SpvReflectInterfaceVariable **outputVariables = (SpvReflectInterfaceVariable **)malloc(
      outputVariableCount * sizeof(SpvReflectInterfaceVariable *));
  verify(spvReflectEnumerateOutputVariables(&reflectModule, &outputVariableCount,
                                            outputVariables) == SPV_REFLECT_RESULT_SUCCESS);
  utarray_alloc(reflect->outputVariables, sizeof(vulkan_shader_reflect_variable *));
  utarray_resize(reflect->outputVariables, outputVariableCount);
  for (size_t idx = 0; idx < outputVariableCount; idx++) {
    SpvReflectInterfaceVariable *spvReflect = outputVariables[idx];
    vulkan_shader_reflect_variable **variable = utarray_eltptr(reflect->outputVariables, idx);
    *variable = vulkan_shader_reflect_variable_create(spvReflect);
  }
  core_free(outputVariables);

  // descriptor bindings
  uint32_t descriptorBindingCount = 0;
  verify(spvReflectEnumerateDescriptorBindings(&reflectModule, &descriptorBindingCount, NULL) ==
         SPV_REFLECT_RESULT_SUCCESS);
  SpvReflectDescriptorBinding **descriptorBindings = (SpvReflectDescriptorBinding **)malloc(
      descriptorBindingCount * sizeof(SpvReflectDescriptorBinding *));
  verify(spvReflectEnumerateDescriptorBindings(&reflectModule, &descriptorBindingCount,
                                               descriptorBindings) == SPV_REFLECT_RESULT_SUCCESS);
  utarray_alloc(reflect->descriptorBindings, sizeof(vulkan_shader_reflect_variable *));
  utarray_resize(reflect->descriptorBindings, descriptorBindingCount);
  for (size_t idx = 0; idx < descriptorBindingCount; idx++) {
    SpvReflectDescriptorBinding *spvReflect = descriptorBindings[idx];
    vulkan_shader_reflect_binding **binding = utarray_eltptr(reflect->descriptorBindings, idx);
    *binding = vulkan_shader_reflect_binding_create(spvReflect);
  }
  core_free(descriptorBindings);

  // TODO: Reflect push constants.
  spvReflectDestroyShaderModule(&reflectModule);

  vulkan_shader_reflect_debug_print(reflect, 0);
  return reflect;
}

void vulkan_shader_reflect_destroy(vulkan_shader_reflect *reflect) {

  utarray_foreach_elem_deref (vulkan_shader_reflect_variable *, inputVariable,
                              reflect->inputVariables) {
    vulkan_shader_reflect_variable_destroy(inputVariable);
  }
  utarray_free(reflect->inputVariables);

  utarray_foreach_elem_deref (vulkan_shader_reflect_variable *, outputVariable,
                              reflect->outputVariables) {
    vulkan_shader_reflect_variable_destroy(outputVariable);
  }
  utarray_free(reflect->outputVariables);

  utarray_foreach_elem_deref (vulkan_shader_reflect_binding *, binding,
                              reflect->descriptorBindings) {
    vulkan_shader_reflect_binding_destroy(binding);
  }
  utarray_free(reflect->descriptorBindings);

  core_free(reflect);
}

void vulkan_shader_reflect_debug_print(vulkan_shader_reflect *reflect, int indent) {
  log_debug(INDENT_FORMAT_STRING "shader reflection:", INDENT_FORMAT_ARGS(0));

  log_debug(INDENT_FORMAT_STRING "input variables count=%zu:", INDENT_FORMAT_ARGS(2),
            utarray_len(reflect->inputVariables));
  utarray_foreach_elem_deref (vulkan_shader_reflect_variable *, inputVariable,
                              reflect->inputVariables) {
    vulkan_shader_reflect_variable_debug_print(inputVariable, 4);
  }

  log_debug(INDENT_FORMAT_STRING "output variables count=%zu:", INDENT_FORMAT_ARGS(2),
            utarray_len(reflect->outputVariables));
  utarray_foreach_elem_deref (vulkan_shader_reflect_variable *, outputVariable,
                              reflect->outputVariables) {
    vulkan_shader_reflect_variable_debug_print(outputVariable, 4);
  }

  log_debug(INDENT_FORMAT_STRING "descriptor binding count=%zu:", INDENT_FORMAT_ARGS(2),
            utarray_len(reflect->descriptorBindings));
  utarray_foreach_elem_deref (vulkan_shader_reflect_binding *, binding,
                              reflect->descriptorBindings) {
    vulkan_shader_reflect_binding_debug_print(binding, 4);
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

void vulkan_shader_reflect_variable_destroy(vulkan_shader_reflect_variable *variable) {
  core_free((char *)variable->name);
  utarray_foreach_elem_deref (vulkan_shader_reflect_variable *, member, variable->members) {
    vulkan_shader_reflect_variable_destroy(member);
  }
  utarray_free(variable->members);
  core_free((char *)variable->typeDescription.type_name);
  core_free((char *)variable->typeDescription.struct_member_name);
  core_free(variable);
}

void vulkan_shader_reflect_variable_debug_print(vulkan_shader_reflect_variable *variable,
                                                int indent) {
  log_debug(INDENT_FORMAT_STRING "variable:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "name: %s", INDENT_FORMAT_ARGS(2), variable->name);
  log_debug(INDENT_FORMAT_STRING "location: %d", INDENT_FORMAT_ARGS(2), variable->location);
  log_debug(INDENT_FORMAT_STRING "storageClass: %s", INDENT_FORMAT_ARGS(2),
            SpvStorageClass__debug_str(variable->storageClass));
  log_debug(INDENT_FORMAT_STRING "builtin: %s", INDENT_FORMAT_ARGS(2),
            SpvBuiltIn__debug_str(variable->builtIn));
  log_debug(INDENT_FORMAT_STRING "numeric: scalarWidth=%u componentCount=%u matrix=%ux%u",
            INDENT_FORMAT_ARGS(2), variable->numeric.scalar.width,
            variable->numeric.vector.component_count, variable->numeric.matrix.column_count,
            variable->numeric.matrix.row_count);
  log_debug(INDENT_FORMAT_STRING "array: dimsCount=%d", INDENT_FORMAT_ARGS(2),
            variable->array.dims_count);
  log_debug(INDENT_FORMAT_STRING "format: %s", INDENT_FORMAT_ARGS(2),
            SpvReflectFormat_debug_str(variable->format));
  log_debug(INDENT_FORMAT_STRING "type: op=%s", INDENT_FORMAT_ARGS(2),
            SpvOp__debug_str(variable->typeDescription.op));
}

vulkan_shader_reflect_binding *
vulkan_shader_reflect_binding_create(SpvReflectDescriptorBinding *reflect) {
  vulkan_shader_reflect_binding *binding = core_alloc(sizeof(vulkan_shader_reflect_binding));

  binding->name = core_strdup(reflect->name);

  binding->binding = reflect->binding;
  binding->inputAttachmentIndex = reflect->input_attachment_index;
  binding->set = reflect->set;

  binding->descriptorType = reflect->descriptor_type;
  binding->resourceType = reflect->resource_type;

  binding->image = reflect->image;
  copy_type_description(&binding->typeDescription, reflect->type_description);

  binding->array = reflect->array;
  binding->count = reflect->count;

  return binding;
}

void vulkan_shader_reflect_binding_destroy(vulkan_shader_reflect_binding *binding) {
  core_free((char *)binding->name);
  core_free((char *)binding->typeDescription.type_name);
  core_free((char *)binding->typeDescription.struct_member_name);
  core_free(binding);
}

void vulkan_shader_reflect_binding_debug_print(vulkan_shader_reflect_binding *binding, int indent) {
  log_debug(INDENT_FORMAT_STRING "binding:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "name: %s", INDENT_FORMAT_ARGS(2), binding->name);
  log_debug(INDENT_FORMAT_STRING "binding: %u", INDENT_FORMAT_ARGS(2), binding->binding);
  log_debug(INDENT_FORMAT_STRING "inputAttachmentIndex: %u", INDENT_FORMAT_ARGS(2),
            binding->inputAttachmentIndex);
  log_debug(INDENT_FORMAT_STRING "set: %u", INDENT_FORMAT_ARGS(2), binding->set);
  log_debug(INDENT_FORMAT_STRING "descriptorType: %u", INDENT_FORMAT_ARGS(2),
            binding->descriptorType);
  log_debug(INDENT_FORMAT_STRING "resourceType: %u", INDENT_FORMAT_ARGS(2), binding->resourceType);
  log_debug(INDENT_FORMAT_STRING "image: dim=%s, imageFormat=%s ", INDENT_FORMAT_ARGS(2),
            SpvDim__debug_str(binding->image.dim),
            SpvImageFormat__debug_str(binding->image.image_format));
  log_debug(INDENT_FORMAT_STRING "type: op=%s", INDENT_FORMAT_ARGS(2),
            SpvOp__debug_str(binding->typeDescription.op));
  log_debug(INDENT_FORMAT_STRING "array: dimsCount=%u", INDENT_FORMAT_ARGS(2),
            binding->array.dims_count);
  log_debug(INDENT_FORMAT_STRING "count: %u", INDENT_FORMAT_ARGS(2), binding->count);
}
