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
  reflect->maxDescriptorSetNumber = 0;
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
    reflect->maxDescriptorSetNumber = MAX(reflect->maxDescriptorSetNumber, (*binding)->setNumber);
  }
  core_free(descriptorBindings);

  // TODO: Reflect push constants.
  spvReflectDestroyShaderModule(&reflectModule);

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
    vulkan_shader_reflect_variable_debug_print(inputVariable, indent + 4);
  }

  log_debug(INDENT_FORMAT_STRING "output variables count=%zu:", INDENT_FORMAT_ARGS(2),
            utarray_len(reflect->outputVariables));
  utarray_foreach_elem_deref (vulkan_shader_reflect_variable *, outputVariable,
                              reflect->outputVariables) {
    vulkan_shader_reflect_variable_debug_print(outputVariable, indent + 4);
  }

  log_debug(INDENT_FORMAT_STRING "descriptor binding count=%zu:", INDENT_FORMAT_ARGS(2),
            utarray_len(reflect->descriptorBindings));
  utarray_foreach_elem_deref (vulkan_shader_reflect_binding *, binding,
                              reflect->descriptorBindings) {
    vulkan_shader_reflect_binding_debug_print(binding, indent + 4);
  }
  log_debug(INDENT_FORMAT_STRING "maxDescriptorSetNumber=%u:", INDENT_FORMAT_ARGS(2),
            reflect->maxDescriptorSetNumber);
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

  variable->typeDesc = vulkan_shader_reflect_type_desc_create(reflect->type_description);
  return variable;
}

void vulkan_shader_reflect_variable_destroy(vulkan_shader_reflect_variable *variable) {
  core_free((char *)variable->name);
  utarray_foreach_elem_deref (vulkan_shader_reflect_variable *, member, variable->members) {
    vulkan_shader_reflect_variable_destroy(member);
  }
  utarray_free(variable->members);
  vulkan_shader_reflect_type_desc_destroy(variable->typeDesc);
  core_free(variable);
}

void vulkan_shader_reflect_variable_debug_print(vulkan_shader_reflect_variable *variable,
                                                int indent) {
  log_debug(INDENT_FORMAT_STRING "variable:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "name: %s", INDENT_FORMAT_ARGS(2), variable->name);
  log_debug(INDENT_FORMAT_STRING "location: %d", INDENT_FORMAT_ARGS(2), variable->location);
  log_debug(INDENT_FORMAT_STRING "storageClass: %s", INDENT_FORMAT_ARGS(2),
            SpvStorageClass__debug_str(variable->storageClass));
  log_debug(INDENT_FORMAT_STRING "builtIn: %s", INDENT_FORMAT_ARGS(2),
            SpvBuiltIn__debug_str(variable->builtIn));
  log_debug(INDENT_FORMAT_STRING "numeric: scalarWidth=%u componentCount=%u matrix=%ux%u",
            INDENT_FORMAT_ARGS(2), variable->numeric.scalar.width,
            variable->numeric.vector.component_count, variable->numeric.matrix.column_count,
            variable->numeric.matrix.row_count);
  log_debug(INDENT_FORMAT_STRING "array: dimsCount=%d", INDENT_FORMAT_ARGS(2),
            variable->array.dims_count);
  log_debug(INDENT_FORMAT_STRING "format: %s", INDENT_FORMAT_ARGS(2),
            SpvReflectFormat_debug_str(variable->format));
  vulkan_shader_reflect_type_desc_debug_print(variable->typeDesc, indent + 2);
}

vulkan_shader_reflect_binding *
vulkan_shader_reflect_binding_create(SpvReflectDescriptorBinding *reflect) {
  vulkan_shader_reflect_binding *binding = core_alloc(sizeof(vulkan_shader_reflect_binding));

  binding->type = vulkan_shader_reflect_binding_type_unknown;
#define strcmp_uniform_buffer(_name, ...)                                                          \
  if (reflect->type_description->type_name != NULL &&                                              \
      strcmp(reflect->type_description->type_name, #_name "Block") == 0)                           \
    binding->type = vulkan_shader_reflect_binding_type_##_name##_uniform_buffer;
  VULKAN_UNIFORM_BUFFERS(strcmp_uniform_buffer, )
#undef strcmp_uniform_buffer
  if (reflect->name != NULL && strncmp(reflect->name, "textures", strlen("textures")) == 0) {
    binding->type = vulkan_shader_reflect_binding_type_textures;
  }
  verify(binding->type != vulkan_shader_reflect_binding_type_unknown &&
         binding->type < vulkan_shader_reflect_binding_type_count);

  binding->binding = reflect->binding;
  binding->inputAttachmentIndex = reflect->input_attachment_index;
  binding->setNumber = reflect->set;

  binding->descriptorType = reflect->descriptor_type;
  binding->resourceType = reflect->resource_type;

  binding->image = reflect->image;
  binding->typeDesc = vulkan_shader_reflect_type_desc_create(reflect->type_description);

  binding->array = reflect->array;
  binding->count = reflect->count;

  return binding;
}

void vulkan_shader_reflect_binding_destroy(vulkan_shader_reflect_binding *binding) {
  vulkan_shader_reflect_type_desc_destroy(binding->typeDesc);
  core_free(binding);
}

VkDescriptorSetLayoutBinding
vulkan_shader_reflect_binding_get_layout_binding(vulkan_shader_reflect_binding *binding,
                                                 vulkan_shader *shader) {
  VkDescriptorSetLayoutBinding layoutBinding = {0};
  layoutBinding.binding = 0;
  layoutBinding.descriptorCount = binding->count;
  if (binding->descriptorType == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER) {
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
  } else if (binding->descriptorType == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  } else if (binding->descriptorType == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
  } else if (binding->descriptorType == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  } else if (binding->descriptorType == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) {
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
  } else if (binding->descriptorType == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER) {
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
  } else if (binding->descriptorType == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  } else if (binding->descriptorType == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  } else if (binding->descriptorType == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) {
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  } else if (binding->descriptorType == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) {
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
  } else if (binding->descriptorType == SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT) {
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
  } else {
    UNREACHABLE;
  }
  layoutBinding.pImmutableSamplers = NULL;
  layoutBinding.stageFlags = vulkan_shader_type_to_stage_flag(shader->type);

  return layoutBinding;
}

void vulkan_shader_reflect_binding_debug_print(vulkan_shader_reflect_binding *binding, int indent) {
  log_debug(INDENT_FORMAT_STRING "binding:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "type: %s", INDENT_FORMAT_ARGS(2),
            vulkan_shader_reflect_binding_type_debug_str(binding->type));
  log_debug(INDENT_FORMAT_STRING "binding: %u", INDENT_FORMAT_ARGS(2), binding->binding);
  log_debug(INDENT_FORMAT_STRING "inputAttachmentIndex: %u", INDENT_FORMAT_ARGS(2),
            binding->inputAttachmentIndex);
  log_debug(INDENT_FORMAT_STRING "setNumber: %u", INDENT_FORMAT_ARGS(2), binding->setNumber);
  log_debug(INDENT_FORMAT_STRING "descriptorType: %u", INDENT_FORMAT_ARGS(2),
            binding->descriptorType);
  log_debug(INDENT_FORMAT_STRING "resourceType: %u", INDENT_FORMAT_ARGS(2), binding->resourceType);
  log_debug(INDENT_FORMAT_STRING "image: dim=%s, imageFormat=%s ", INDENT_FORMAT_ARGS(2),
            SpvDim__debug_str(binding->image.dim),
            SpvImageFormat__debug_str(binding->image.image_format));
  vulkan_shader_reflect_type_desc_debug_print(binding->typeDesc, indent + 2);
  log_debug(INDENT_FORMAT_STRING "array: dimsCount=%u", INDENT_FORMAT_ARGS(2),
            binding->array.dims_count);
  log_debug(INDENT_FORMAT_STRING "count: %u", INDENT_FORMAT_ARGS(2), binding->count);
}

/* vulkan_shader_type_desc */

vulkan_shader_reflect_type_desc *
vulkan_shader_reflect_type_desc_create(SpvReflectTypeDescription *reflect) {
  vulkan_shader_reflect_type_desc *typeDesc = core_alloc(sizeof(vulkan_shader_reflect_type_desc));
  typeDesc->op = reflect->op;
  typeDesc->typeName = core_strdup(reflect->type_name);
  typeDesc->structMemberName = core_strdup(reflect->struct_member_name);
  typeDesc->storageClass = reflect->storage_class;
  typeDesc->typeFlags = reflect->type_flags;
  typeDesc->decorationFlags = reflect->decoration_flags;

  typeDesc->numeric = reflect->traits.numeric;
  typeDesc->image = reflect->traits.image;
  typeDesc->array = reflect->traits.array;

  utarray_alloc(typeDesc->members, sizeof(vulkan_shader_reflect_type_desc *));
  utarray_resize(typeDesc->members, reflect->member_count);
  for (size_t idx = 0; idx < utarray_len(typeDesc->members); idx++) {
    vulkan_shader_reflect_type_desc **member = utarray_eltptr(typeDesc->members, idx);
    *member = vulkan_shader_reflect_type_desc_create(&reflect->members[idx]);
  }

  return typeDesc;
}

void vulkan_shader_reflect_type_desc_destroy(vulkan_shader_reflect_type_desc *typeDesc) {
  core_free((char *)typeDesc->typeName);
  core_free((char *)typeDesc->structMemberName);
  utarray_foreach_elem_deref (vulkan_shader_reflect_type_desc *, member, typeDesc->members) {
    vulkan_shader_reflect_type_desc_destroy(member);
  }
  utarray_free(typeDesc->members);
  core_free(typeDesc);
}

void vulkan_shader_reflect_type_desc_debug_print(vulkan_shader_reflect_type_desc *typeDesc,
                                                 int indent) {
  log_debug(INDENT_FORMAT_STRING "typeDesc:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "op: %s", INDENT_FORMAT_ARGS(2), SpvOp__debug_str(typeDesc->op));
  log_debug(INDENT_FORMAT_STRING "typeName: %s", INDENT_FORMAT_ARGS(2), typeDesc->typeName);
  log_debug(INDENT_FORMAT_STRING "structMemberName: %s", INDENT_FORMAT_ARGS(2),
            typeDesc->structMemberName);
  log_debug(INDENT_FORMAT_STRING "storageClass: %s", INDENT_FORMAT_ARGS(2),
            SpvStorageClass__debug_str(typeDesc->storageClass));
  log_debug(INDENT_FORMAT_STRING "typeFlags: %s", INDENT_FORMAT_ARGS(2),
            SpvReflectTypeFlagBits_debug_str(typeDesc->typeFlags));
  log_debug(INDENT_FORMAT_STRING "decorationFlags:", INDENT_FORMAT_ARGS(2));
  SpvReflectDecorationFlagBits_debug_print(typeDesc->decorationFlags, indent + 4);

  log_debug(INDENT_FORMAT_STRING "numeric: scalarWidth=%u componentCount=%u matrix=%ux%u",
            INDENT_FORMAT_ARGS(2), typeDesc->numeric.scalar.width,
            typeDesc->numeric.vector.component_count, typeDesc->numeric.matrix.column_count,
            typeDesc->numeric.matrix.row_count);
  log_debug(INDENT_FORMAT_STRING "image: dim=%s, imageFormat=%s ", INDENT_FORMAT_ARGS(2),
            SpvDim__debug_str(typeDesc->image.dim),
            SpvImageFormat__debug_str(typeDesc->image.image_format));
  log_debug(INDENT_FORMAT_STRING "array: dimsCount=%d", INDENT_FORMAT_ARGS(2),
            typeDesc->array.dims_count);

  log_debug(INDENT_FORMAT_STRING "members count=%zu:", INDENT_FORMAT_ARGS(2),
            utarray_len(typeDesc->members));
  utarray_foreach_elem_deref (vulkan_shader_reflect_type_desc *, member, typeDesc->members) {
    vulkan_shader_reflect_type_desc_debug_print(member, indent + 2);
  }
}
