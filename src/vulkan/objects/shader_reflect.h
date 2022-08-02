/* GLSL shader to SPIR-V binary compiler */
#pragma once

#include "descriptor.h"
#include "shader.h"

/// Describes SPIR-V shader reflection.
typedef struct shader_reflect {
  UT_array *inputVariables;  ///< shader_reflect_variable* array.
  UT_array *outputVariables; ///< shader_reflect_variable* array.

  UT_array *descriptorBindings;    ///< shader_reflect_binding* array.
  uint32_t maxDescriptorSetNumber; ///< Max descriptor set number referred to by shader bindings.
} shader_reflect;

shader_reflect *shader_reflect_create(uint32_t *spvCode, size_t spvSize);
void shader_reflect_destroy(shader_reflect *reflect);

void shader_reflect_debug_print(shader_reflect *reflect, int indent);

typedef struct shader_reflect_type_desc shader_reflect_type_desc;

typedef struct shader_reflect_variable {
  const char *name;
  uint32_t location;
  SpvStorageClass storageClass;
  SpvBuiltIn builtIn;
  SpvReflectNumericTraits numeric;
  SpvReflectArrayTraits array;

  UT_array *members; /// shader_reflect_variable* list.

  SpvReflectFormat format;

  shader_reflect_type_desc *typeDesc;
} shader_reflect_variable;

shader_reflect_variable *shader_reflect_variable_create(SpvReflectInterfaceVariable *reflect);
void shader_reflect_variable_destroy(shader_reflect_variable *inputVariable);
void shader_reflect_variable_debug_print(shader_reflect_variable *inputVariable, int indent);

typedef enum shader_reflect_binding_type {
  shader_reflect_binding_type_unknown,
#define enum_uniform_buffer(_name, ...) shader_reflect_binding_type_##_name##_uniform_buffer,
  VULKAN_UNIFORM_BUFFERS(enum_uniform_buffer, )
#undef enum_uniform_buffer
      shader_reflect_binding_type_textures,
  shader_reflect_binding_type_count,
} shader_reflect_binding_type;

typedef struct shader_reflect_binding {
  shader_reflect_binding_type type;
  uint32_t location;

  uint32_t binding;
  uint32_t inputAttachmentIndex;
  uint32_t setNumber;

  SpvReflectDescriptorType descriptorType;
  SpvReflectResourceType resourceType;

  SpvReflectImageTraits image;
  shader_reflect_type_desc *typeDesc;

  SpvReflectBindingArrayTraits array;
  uint32_t count;
} shader_reflect_binding;

shader_reflect_binding *shader_reflect_binding_create(SpvReflectDescriptorBinding *reflect);
void shader_reflect_binding_destroy(shader_reflect_binding *binding);

VkDescriptorSetLayoutBinding
shader_reflect_binding_get_layout_binding(shader_reflect_binding *binding, shader *shader);

void shader_reflect_binding_debug_print(shader_reflect_binding *binding, int indent);

typedef struct shader_reflect_type_desc {
  SpvOp op;
  const char *typeName;
  const char *structMemberName;
  SpvStorageClass storageClass;
  SpvReflectTypeFlags typeFlags;
  SpvReflectDecorationFlags decorationFlags;

  SpvReflectNumericTraits numeric;
  SpvReflectImageTraits image;
  SpvReflectArrayTraits array;

  UT_array *members; /// shader_reflect_type_desc* list.
} shader_reflect_type_desc;

shader_reflect_type_desc *shader_reflect_type_desc_create(SpvReflectTypeDescription *reflect);
void shader_reflect_type_desc_destroy(shader_reflect_type_desc *typeDesc);
void shader_reflect_type_desc_debug_print(shader_reflect_type_desc *typeDesc, int indent);
