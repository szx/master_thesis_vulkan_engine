/* GLSL shader to SPIR-V binary compiler */
#pragma once

#include "descriptor.h"
#include "shader.h"

/// Describes SPIR-V shader reflection.
typedef struct vulkan_shader_reflect {
  UT_array *inputVariables;  ///< vulkan_shader_reflect_variable* array.
  UT_array *outputVariables; ///< vulkan_shader_reflect_variable* array.

  UT_array *descriptorBindings;    ///< vulkan_shader_reflect_binding* array.
  uint32_t maxDescriptorSetNumber; ///< Max descriptor set number referred to by shader bindings.
} vulkan_shader_reflect;

vulkan_shader_reflect *vulkan_shader_reflect_create(uint32_t *spvCode, size_t spvSize);
void vulkan_shader_reflect_destroy(vulkan_shader_reflect *reflect);

void vulkan_shader_reflect_debug_print(vulkan_shader_reflect *reflect, int indent);

typedef struct vulkan_shader_reflect_type_desc vulkan_shader_reflect_type_desc;

typedef struct vulkan_shader_reflect_variable {
  const char *name;
  uint32_t location;
  SpvStorageClass storageClass;
  SpvBuiltIn builtIn;
  SpvReflectNumericTraits numeric;
  SpvReflectArrayTraits array;

  UT_array *members; /// vulkan_shader_reflect_variable* list.

  SpvReflectFormat format;

  vulkan_shader_reflect_type_desc *typeDesc;
} vulkan_shader_reflect_variable;

vulkan_shader_reflect_variable *
vulkan_shader_reflect_variable_create(SpvReflectInterfaceVariable *reflect);
void vulkan_shader_reflect_variable_destroy(vulkan_shader_reflect_variable *inputVariable);
void vulkan_shader_reflect_variable_debug_print(vulkan_shader_reflect_variable *inputVariable,
                                                int indent);

typedef enum vulkan_shader_reflect_binding_type {
  vulkan_shader_reflect_binding_type_unknown,
#define enum_uniform_buffer(_name, ...) vulkan_shader_reflect_binding_type_##_name##_uniform_buffer,
  VULKAN_UNIFORM_BUFFERS(enum_uniform_buffer, )
#undef enum_uniform_buffer
      vulkan_shader_reflect_binding_type_count,
} vulkan_shader_reflect_binding_type;

typedef struct vulkan_shader_reflect_binding {
  vulkan_shader_reflect_binding_type type;
  uint32_t location;

  uint32_t binding;
  uint32_t inputAttachmentIndex;
  uint32_t setNumber;

  SpvReflectDescriptorType descriptorType;
  SpvReflectResourceType resourceType;

  SpvReflectImageTraits image;
  vulkan_shader_reflect_type_desc *typeDesc;

  SpvReflectBindingArrayTraits array;
  uint32_t count;
} vulkan_shader_reflect_binding;

vulkan_shader_reflect_binding *
vulkan_shader_reflect_binding_create(SpvReflectDescriptorBinding *reflect);
void vulkan_shader_reflect_binding_destroy(vulkan_shader_reflect_binding *binding);

VkDescriptorSetLayoutBinding
vulkan_shader_reflect_binding_get_layout_binding(vulkan_shader_reflect_binding *binding,
                                                 vulkan_shader *shader);

void vulkan_shader_reflect_binding_debug_print(vulkan_shader_reflect_binding *binding, int indent);

typedef struct vulkan_shader_reflect_type_desc {
  SpvOp op;
  const char *typeName;
  const char *structMemberName;
  SpvStorageClass storageClass;
  SpvReflectTypeFlags typeFlags;
  SpvReflectDecorationFlags decorationFlags;

  SpvReflectNumericTraits numeric;
  SpvReflectImageTraits image;
  SpvReflectArrayTraits array;

  UT_array *members; /// vulkan_shader_reflect_type_desc* list.
} vulkan_shader_reflect_type_desc;

vulkan_shader_reflect_type_desc *
vulkan_shader_reflect_type_desc_create(SpvReflectTypeDescription *reflect);
void vulkan_shader_reflect_type_desc_destroy(vulkan_shader_reflect_type_desc *typeDesc);
void vulkan_shader_reflect_type_desc_debug_print(vulkan_shader_reflect_type_desc *typeDesc,
                                                 int indent);
