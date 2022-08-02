/* GLSL shader to SPIR-V binary compiler */
#pragma once

#include "descriptor.h"
#include "vertex_stream.h"

typedef struct shader_reflect shader_reflect;

typedef enum shader_type {
  shader_type_unknown,
  shader_type_vertex,
  shader_type_fragment,
  shader_type_count,
} shader_type;

VkShaderStageFlags shader_type_to_stage_flag(shader_type type);

/// Describes compiled GLSL shader.
typedef struct shader {
  shader_type type;
  uint32_t *spvCode;
  UT_string *glslCode;
  size_t spvSize;
  VkShaderModule module;
  /* reflection */
  shader_reflect *reflect;
  /* Vulkan objects */
  device *vkd; ///< Pointer.
} shader;

shader *shader_create_with_str(device *vkd, shader_type type, UT_string *text);
void shader_destroy(shader *shader);

void shader_debug_print(shader *shader, int indent);
