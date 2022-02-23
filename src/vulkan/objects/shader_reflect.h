/* GLSL shader to SPIR-V binary compiler */
#pragma once

#include "common.h"
#include "descriptor.h"
#include "vertex_stream.h"

/// Describes SPIR-V shader reflection.
typedef struct vulkan_shader_reflect {
  UT_array *inputVariables;     /// vulkan_shader_reflect_variable* list.
  UT_array *outputVariables;    /// vulkan_shader_reflect_variable* list.
  UT_array *descriptorBindings; /// vulkan_shader_reflect_binding* list.
} vulkan_shader_reflect;

vulkan_shader_reflect *vulkan_shader_reflect_create(uint32_t *spvCode, size_t spvSize);
void vulkan_shader_reflect_destroy(vulkan_shader_reflect *reflect);

void vulkan_shader_reflect_debug_print(vulkan_shader_reflect *reflect, int indent);

typedef struct vulkan_shader_reflect_variable {
  const char *name;
  uint32_t location;
  SpvStorageClass storageClass;
  SpvBuiltIn builtIn;
  SpvReflectNumericTraits numeric;
  SpvReflectArrayTraits array;

  UT_array *members; /// vulkan_shader_reflect_variable* list.

  SpvReflectFormat format;

  SpvReflectTypeDescription typeDescription;
} vulkan_shader_reflect_variable;

vulkan_shader_reflect_variable *
vulkan_shader_reflect_variable_create(SpvReflectInterfaceVariable *reflect);
void vulkan_shader_reflect_variable_destroy(vulkan_shader_reflect_variable *inputVariable);
void vulkan_shader_reflect_variable_debug_print(vulkan_shader_reflect_variable *inputVariable,
                                                int indent);

typedef struct vulkan_shader_reflect_binding {
  const char *name;
  uint32_t location;

  uint32_t binding;
  uint32_t inputAttachmentIndex;
  uint32_t set;

  SpvReflectDescriptorType descriptorType;
  SpvReflectResourceType resourceType;

  SpvReflectImageTraits image;
  SpvReflectTypeDescription typeDescription;

  SpvReflectBindingArrayTraits array;
  uint32_t count;
} vulkan_shader_reflect_binding;

vulkan_shader_reflect_binding *
vulkan_shader_reflect_binding_create(SpvReflectDescriptorBinding *reflect);
void vulkan_shader_reflect_binding_destroy(vulkan_shader_reflect_binding *inputVariable);
void vulkan_shader_reflect_binding_debug_print(vulkan_shader_reflect_binding *inputVariable,
                                               int indent);
