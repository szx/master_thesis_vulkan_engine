/* GLSL shader to SPIR-V binary compiler */
#pragma once

#include "common.h"
#include "descriptor.h"
#include "vertex_stream.h"

typedef enum vulkan_shader_type {
  vulkan_shader_type_unknown,
  vulkan_shader_type_vertex,
  vulkan_shader_type_fragment,
  vulkan_shader_type_count,
} vulkan_shader_type;

// HIRO vulkan shader generation (use descriptor codegen)

/// Describes compiled GLSL shader.
typedef struct vulkan_shader {
  vulkan_device *vkd; /// Pointer.
  vulkan_shader_type type;
  uint32_t *spvCode;

  UT_string *glslCode;
  size_t spvSize;
  VkShaderModule module;

  /* reflection */
  // HIRO: SPIRV-Reflect
} vulkan_shader;

// HIRO input should be vulkan_shader_data?
// HIRO use in render pass
vulkan_shader *vulkan_shader_create_with_str(vulkan_device *vkd, vulkan_shader_type type,
                                             UT_string *text);
void vulkan_shader_destroy(vulkan_shader *shader);
