/* GLSL shader to SPIR-V binary compiler */
#pragma once

#include "common.h"
#include "descriptor.h"
#include "vertex_stream.h"

typedef struct vulkan_shader_reflect vulkan_shader_reflect;

typedef enum vulkan_shader_type {
  vulkan_shader_type_unknown,
  vulkan_shader_type_vertex,
  vulkan_shader_type_fragment,
  vulkan_shader_type_count,
} vulkan_shader_type;

VkShaderStageFlags vulkan_shader_type_to_stage_flag(vulkan_shader_type type);

/// Describes compiled GLSL shader.
typedef struct vulkan_shader {
  vulkan_shader_type type;
  uint32_t *spvCode;
  UT_string *glslCode;
  size_t spvSize;
  VkShaderModule module;
  /* reflection */
  vulkan_shader_reflect *reflect;
  /* Vulkan objects */
  vulkan_device *vkd; ///< Pointer.
} vulkan_shader;

vulkan_shader *vulkan_shader_create_with_str(vulkan_device *vkd, vulkan_shader_type type,
                                             UT_string *text);
void vulkan_shader_destroy(vulkan_shader *shader);

void vulkan_shader_debug_print(vulkan_shader *shader, int indent);
