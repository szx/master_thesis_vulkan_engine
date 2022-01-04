/* GLSL shader to SPIR-V binary compiler */

#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include "device.h"
#include "functions.h"
#include "scene.h"

/// Describes one vertex input (or output) attribute defined in GLSL shader.
/// Examples:
/// layout(location = 0) in vec2 inPosition;
/// layout(location = 0) out vec3 outColor;
typedef struct vulkan_vertex_attribute_description {
  size_t location;
  size_t componentNum;
  char *identifier;
  vulkan_attribute_type type;
} vulkan_vertex_attribute_description;

/// Describes one push constant in GLSL shader.
/// Examples:
/// layout(push_constant) uniform BlockName
/// {
///     vec4 data;
///     mat4 render_matrix;
/// } InstanceName;
typedef struct vulkan_push_constant_description {
  char *blockName;
  char *instanceName;
  size_t size;
} vulkan_push_constant_description;

/// Describes results of parsing GLSL shader for information.
/// Does not contain scene-dependant information (like vertex attribute offsets).
typedef struct vulkan_shader_info {
  core_array(vulkan_vertex_attribute_description) inputAttributeDescriptions;
  core_array(vulkan_vertex_attribute_description) outputAttributeDescriptions;
  vulkan_push_constant_description *pushConstantDescription;
} vulkan_shader_info;

/// Describes compiled GLSL shader.
typedef struct vulkan_shader {
  vulkan_device *vkd; /// vulkan_device pointer.
  str glslCode;
  shaderc_shader_kind type;
  uint32_t *spvCode;
  size_t spvSize;
  VkShaderModule module;
  vulkan_shader_info info;
} vulkan_shader;

void vulkan_vertex_attribute_description_init(vulkan_vertex_attribute_description *description,
                                              size_t location, size_t componentNum,
                                              const char *identifier, vulkan_attribute_type type);
void vulkan_vertex_attribute_description_deinit(vulkan_vertex_attribute_description *description);

vulkan_push_constant_description *vulkan_push_constant_description_create(const char *blockName,
                                                                          const char *instanceName,
                                                                          size_t size);
void vulkan_push_constant_description_destroy(vulkan_push_constant_description *description);

void vulkan_shader_info_init(vulkan_shader_info *info, vulkan_shader *shader);
void vulkan_shader_info_deinit(vulkan_shader_info *info);

size_t vulkan_shader_info_get_binding_count(vulkan_shader_info *info);
VkVertexInputBindingDescription
vulkan_shader_info_get_binding_description(vulkan_shader_info *info);
/// Returns vertex attribute descriptions.
/// We expect that geometry buffer on device has interleaved vertex attributes.
VkVertexInputAttributeDescription *
vulkan_shader_info_get_attribute_descriptions(vulkan_shader_info *info, size_t *count);
VkPushConstantRange vulkan_shader_info_get_push_constant_range(vulkan_shader *vertShader,
                                                               vulkan_shader *fragShader);
VkShaderStageFlagBits vulkan_shader_info_get_push_constant_stage_flags(vulkan_shader *vertShader,
                                                                       vulkan_shader *fragShader);

vulkan_shader *vulkan_shader_create_with_path(vulkan_device *vkd, platform_path glslPath);
vulkan_shader *vulkan_shader_create_with_str(vulkan_device *vkd, shaderc_shader_kind type,
                                             str *text);
void vulkan_shader_destroy(vulkan_shader *shader);

#include "../codegen/shader.h"

#endif /* !VULKAN_SHADER */
