/* GLSL shader to SPIR-V binary compiler */

#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include "functions.h"

/// Describes one vertex input (or output) attribute defined in GLSL shader.
/// Examples:
/// layout(location = 0) in vec2 inPosition;
/// layout(location = 0) out vec3 fragColor;
typedef struct vulkan_vertex_attribute_description {
  size_t location;
  size_t componentNum;
  char *identifier;
} vulkan_vertex_attribute_description;

/// Describes results of parsing GLSL shader for information.
typedef struct vulkan_shader_info {
  vulkan_vertex_attribute_description *inputAttributeDescriptions;
  vulkan_vertex_attribute_description *outputAttributeDescriptions;
} vulkan_shader_info;

/// Describes compiled GLSL shader.
typedef struct vulkan_shader {
  vulkan_device *vkd; /// vulkan_device pointer.
  platform_path glslPath;
  char *glslCode;
  size_t glslSize;
  shaderc_shader_kind type;
  uint32_t *spvCode;
  size_t spvSize;
  VkShaderModule module;
  vulkan_shader_info info;
} vulkan_shader;

void vulkan_vertex_attribute_description_init(vulkan_vertex_attribute_description *description,
                                              size_t location, size_t componentNum,
                                              const char *identifier);
void vulkan_vertex_attribute_description_deinit(vulkan_vertex_attribute_description *description);

void vulkan_shader_info_init(vulkan_shader_info *info, vulkan_shader *shader);
void vulkan_shader_info_deinit(vulkan_shader_info *info);
VkVertexInputBindingDescription
vulkan_shader_info_get_binding_description(vulkan_shader_info *info);
VkVertexInputAttributeDescription *
vulkan_shader_info_get_attribute_descriptions(vulkan_shader_info *info, size_t *count);

void vulkan_shader_init(vulkan_shader *shader, vulkan_device *vkd, platform_path glslPath);
void vulkan_shader_deinit(vulkan_shader *shader);

#include "../codegen/shader.h"

#endif /* !VULKAN_SHADER */
