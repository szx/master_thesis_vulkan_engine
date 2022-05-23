/* C structs used by shaders in uniform buffers. */

#pragma once

#include "buffer.h"

typedef uint32_t uint;

typedef struct vulkan_draw_push_constant_struct {
  uint currentFrameInFlight;
} vulkan_draw_push_constant_struct;

typedef struct vulkan_material_helper_struct {
  uint baseColorTextureId;
  vec4 baseColorFactor;
  uint metallicRoughnessTextureId;
  float metallicFactor;
  float roughnessFactor;
} vulkan_material_helper_struct;

typedef struct vulkan_directional_light_helper_struct {
  vec3 direction;
  vec3 color;
} vulkan_directional_light_helper_struct;

typedef struct vulkan_point_light_helper_struct {
  vec3 position;
  vec3 color;
  float radius;
} vulkan_point_light_helper_struct;

typedef struct vulkan_skybox_helper_struct {
  uint skyboxCubemapTextureId;
} vulkan_skybox_helper_struct;

typedef struct vulkan_font_helper_struct {
  uint fontTextureId;
  uint characterSize;
  uint fontTextureSize;
  uint textLength;
  uint text; ///< array=MAX_TEXT_CHARACTER_COUNT
} vulkan_font_helper_struct;

typedef struct vulkan_viewport_helper_struct {
  uint width;
  uint height;
} vulkan_viewport_helper_struct;

typedef struct vulkan_g_buffer_helper_struct {
  uint gBuffer0TextureId;
  uint gBuffer1TextureId;
  uint gBuffer2TextureId;
} vulkan_g_buffer_helper_struct;

typedef struct vulkan_global_uniform_buffer_struct {
  mat4 viewMat;
  mat4 projMat;
  uint materialCount;
  vulkan_material_helper_struct materials; ///< array=MAX_MATERIAL_COUNT
  uint directionalLightCount;
  vulkan_directional_light_helper_struct directionalLights; ///< array=MAX_DIRECTIONAL_LIGHT_COUNT
  uint pointLightCount;
  vulkan_point_light_helper_struct pointLights; ///< array=MAX_POINT_LIGHT_COUNT
  vulkan_skybox_helper_struct skybox;
  vulkan_font_helper_struct font;
  vulkan_viewport_helper_struct viewport;
  vulkan_g_buffer_helper_struct gBuffer;
} vulkan_global_uniform_buffer_struct;

typedef struct vulkan_instances_uniform_buffer_struct {
  mat4 modelMat;
  uint materialId;
} vulkan_instances_uniform_buffer_struct;

// *_uniform_element
// VULKAN_UNIFORM_BUFFERS
#include "../../codegen/descriptors.h"

// VULKAN_UNIFORM_BUFFER_COUNT
#define VULKAN_UNIFORM_BUFFER_COUNT_(_name, ...) +1
#define VULKAN_UNIFORM_BUFFER_COUNT (0 VULKAN_UNIFORM_BUFFERS(VULKAN_UNIFORM_BUFFER_COUNT_, ))

// *_uniform_buffer_data
// NOTE: Stores FRAMES_IN_FLIGHT*count elements, element n for frame #0, n+1 for frame #1 etc.
#define decl_uniform_buffer_data(_name, ...)                                                       \
  typedef struct vulkan_##_name##_uniform_buffer_data {                                            \
    vulkan_buffer_element bufferElement;                                                           \
    uint32_t frames;                                                                               \
    uint32_t count;                                                                                \
    vulkan_##_name##_uniform_buffer_element elements[];                                            \
  } vulkan_##_name##_uniform_buffer_data;                                                          \
  vulkan_##_name##_uniform_buffer_data *vulkan_##_name##_uniform_buffer_data_create(               \
      uint32_t count, uint32_t frames);                                                            \
  void vulkan_##_name##_uniform_buffer_data_destroy(                                               \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer);                                        \
  size_t vulkan_##_name##_uniform_buffer_data_get_count(                                           \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer);                                        \
  size_t vulkan_##_name##_uniform_buffer_data_get_size(                                            \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer);                                        \
  vulkan_##_name##_uniform_buffer_element *vulkan_##_name##_uniform_buffer_data_get_element(       \
      vulkan_##_name##_uniform_buffer_data *uniformBuffer, size_t index,                           \
      size_t currentFrameInFlight);
VULKAN_UNIFORM_BUFFERS(decl_uniform_buffer_data, )
#undef decl_uniform_buffer_data

typedef struct vulkan_descriptors vulkan_descriptors;
typedef struct vulkan_unified_uniform_buffer vulkan_unified_uniform_buffer;
typedef struct vulkan_textures vulkan_textures;

/// Describes one descriptor binding in descriptor set.
typedef struct vulkan_descriptor_binding {
  vulkan_descriptors *descriptors;
  uint32_t bindingNumber;
  uint32_t descriptorCount;
  VkDescriptorType descriptorType;
  union {
    vulkan_buffer_element *bufferElement;
    vulkan_textures *textures;
  };
} vulkan_descriptor_binding;

VkDescriptorSetLayout vulkan_create_descriptor_set_layout(vulkan_device *vkd,
                                                          vulkan_descriptor_binding *bindings,
                                                          size_t bindingCount, bool bindless,
                                                          const char *debugFormat, ...);

VkDescriptorSet
vulkan_create_descriptor_set(vulkan_device *vkd, VkDescriptorSetLayout descriptorSetLayout,
                             VkDescriptorPool descriptorPool, vulkan_descriptor_binding *bindings,
                             size_t bindingCount, bool bindless, const char *debugFormat, ...);

void vulkan_update_descriptor_set(vulkan_device *vkd, VkDescriptorSet descriptorSet,
                                  vulkan_descriptor_binding *bindings, size_t bindingCount);

void vulkan_descriptor_binding_debug_print(vulkan_descriptor_binding *binding, int indent);

/// Descriptors used to bind resources (uniform buffers and textures) to shaders.
/// Uniform buffers use classic slot-based binding model.
/// Textures use bindless model.
typedef struct vulkan_descriptors {
  vulkan_device *vkd;                                  ///< Pointer.
  vulkan_unified_uniform_buffer *unifiedUniformBuffer; ///< Pointer.
  vulkan_textures *textures;                           ///< Pointer.

  /* uniform buffers */
  /// Descriptor pool used to allocate descriptors for resources used by shaders.
  VkDescriptorPool descriptorPool;

  /// One giant descriptor set layout per one shader binding (layout qualifier).
  VkDescriptorSetLayout descriptorSetLayout;

  /// One giant descriptor set with one binding per resource.
  VkDescriptorSet descriptorSet;
  uint32_t descriptorSetNumber;

  /// Pipeline layout for descriptor set.
  VkPipelineLayout pipelineLayout;

  vulkan_descriptor_binding uniformBufferBindings[VULKAN_UNIFORM_BUFFER_COUNT];
  vulkan_descriptor_binding texturesBinding;

  /// Pointers to named uniform buffer binding.
#define decl_uniform_buffer(_name, ...) vulkan_descriptor_binding *_name##UniformBufferBinding;
  VULKAN_UNIFORM_BUFFERS(decl_uniform_buffer, )
#undef decl_uniform_buffer

} vulkan_descriptors;

vulkan_descriptors *vulkan_descriptors_create(vulkan_device *vkd,
                                              vulkan_unified_uniform_buffer *unifiedUniformBuffer,
                                              vulkan_textures *textures);

void vulkan_descriptors_destroy(vulkan_descriptors *descriptors);

void vulkan_descriptors_update(vulkan_descriptors *descriptors);

void vulkan_descriptors_send_to_device(vulkan_descriptors *descriptors);

void vulkan_descriptors_record_bind_commands(vulkan_descriptors *descriptors,
                                             VkCommandBuffer commandBuffer,
                                             vulkan_draw_push_constant_element drawPushConstant);
VkDescriptorSetLayout *
vulkan_descriptors_get_descriptor_set_layouts(vulkan_descriptors *descriptors, size_t *count);

void vulkan_descriptors_debug_print(vulkan_descriptors *descriptors, int indent);
