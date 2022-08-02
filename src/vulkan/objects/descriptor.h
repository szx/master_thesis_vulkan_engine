/* C structs used by shaders in uniform buffers. */

#pragma once

#include "buffer.h"

typedef uint32_t uint;

typedef struct draw_push_constant_struct {
  uint currentFrameInFlight;
} draw_push_constant_struct;

typedef struct material_helper_struct {
  uint baseColorTextureId;
  vec4 baseColorFactor;
  uint metallicRoughnessTextureId;
  uint normalMapTextureId;
  float metallicFactor;
  float roughnessFactor;
} material_helper_struct;

typedef struct directional_light_helper_struct {
  vec3 direction;
  vec3 color;
  float intensity;
} directional_light_helper_struct;

typedef struct point_light_helper_struct {
  vec3 position;
  vec3 color;
  float range;
  float intensity;
} point_light_helper_struct;

typedef struct spot_light_helper_struct {
  vec3 position;
  vec3 color;
  float innerConeAngle;
  float outerConeAngle;
  float range;
  float intensity;
} spot_light_helper_struct;

typedef struct skybox_helper_struct {
  uint skyboxCubemapTextureId;
} skybox_helper_struct;

typedef struct font_helper_struct {
  uint fontTextureId;
  uint characterSize;
  uint fontTextureSize;
  uint textLength;
  uint text; ///< array=MAX_TEXT_CHARACTER_COUNT
} font_helper_struct;

typedef struct viewport_helper_struct {
  uint width;
  uint height;
} viewport_helper_struct;

typedef struct offscreen_texture_helper_struct {
  uint textureId; ///< array=MAX_OFFSCREEN_TEXTURE_COUNT
} offscreen_texture_helper_struct;

typedef struct global_uniform_buffer_struct {
  mat4 viewMat;
  mat4 projMat;
  uint materialCount;
  material_helper_struct materials; ///< array=MAX_MATERIAL_COUNT
  uint directionalLightCount;
  directional_light_helper_struct directionalLights; ///< array=MAX_DIRECTIONAL_LIGHT_COUNT
  uint pointLightCount;
  point_light_helper_struct pointLights; ///< array=MAX_POINT_LIGHT_COUNT
  uint spotLightCount;
  spot_light_helper_struct spotLights; ///< array=MAX_SPOT_LIGHT_COUNT
  skybox_helper_struct skybox;
  font_helper_struct font;
  viewport_helper_struct viewport;
  offscreen_texture_helper_struct offscreenTextures;
} global_uniform_buffer_struct;

typedef struct instances_uniform_buffer_struct {
  mat4 modelMat;
  uint materialId;
} instances_uniform_buffer_struct;

// *_uniform_element
// VULKAN_UNIFORM_BUFFERS
#include "../../codegen/descriptors.h"

// VULKAN_UNIFORM_BUFFER_COUNT
#define VULKAN_UNIFORM_BUFFER_COUNT_(_name, ...) +1
#define VULKAN_UNIFORM_BUFFER_COUNT (0 VULKAN_UNIFORM_BUFFERS(VULKAN_UNIFORM_BUFFER_COUNT_, ))

// *_uniform_buffer_data
// NOTE: Stores FRAMES_IN_FLIGHT*count elements, element n for frame #0, n+1 for frame #1 etc.
#define decl_uniform_buffer_data(_name, ...)                                                       \
  typedef struct _name##_uniform_buffer_data {                                                     \
    buffer_element bufferElement;                                                                  \
    uint32_t frames;                                                                               \
    uint32_t count;                                                                                \
    _name##_uniform_buffer_element elements[];                                                     \
  } _name##_uniform_buffer_data;                                                                   \
  _name##_uniform_buffer_data *_name##_uniform_buffer_data_create(uint32_t count,                  \
                                                                  uint32_t frames);                \
  void _name##_uniform_buffer_data_destroy(_name##_uniform_buffer_data *uniformBuffer);            \
  size_t _name##_uniform_buffer_data_get_count(_name##_uniform_buffer_data *uniformBuffer);        \
  size_t _name##_uniform_buffer_data_get_size(_name##_uniform_buffer_data *uniformBuffer);         \
  _name##_uniform_buffer_element *_name##_uniform_buffer_data_get_element(                         \
      _name##_uniform_buffer_data *uniformBuffer, size_t index, size_t currentFrameInFlight);
VULKAN_UNIFORM_BUFFERS(decl_uniform_buffer_data, )
#undef decl_uniform_buffer_data

typedef struct descriptors descriptors;
typedef struct unified_uniform_buffer unified_uniform_buffer;
typedef struct textures textures;

/// Describes one descriptor binding in descriptor set.
typedef struct descriptor_binding {
  descriptors *descriptors;
  uint32_t bindingNumber;
  uint32_t descriptorCount;
  VkDescriptorType descriptorType;
  union {
    buffer_element *bufferElement;
    textures *textures;
  };
} descriptor_binding;

VkDescriptorSetLayout create_descriptor_set_layout(device *vkd, descriptor_binding *bindings,
                                                   size_t bindingCount, bool bindless,
                                                   const char *debugFormat, ...);

VkDescriptorSet create_descriptor_set(device *vkd, VkDescriptorSetLayout descriptorSetLayout,
                                      VkDescriptorPool descriptorPool, descriptor_binding *bindings,
                                      size_t bindingCount, bool bindless, const char *debugFormat,
                                      ...);

void update_descriptor_set(device *vkd, VkDescriptorSet descriptorSet, descriptor_binding *bindings,
                           size_t bindingCount);

void descriptor_binding_debug_print(descriptor_binding *binding, int indent);

/// Descriptors used to bind resources (uniform buffers and textures) to shaders.
/// Uniform buffers use classic slot-based binding model.
/// Textures use bindless model.
typedef struct descriptors {
  device *vkd;                                  ///< Pointer.
  unified_uniform_buffer *unifiedUniformBuffer; ///< Pointer.
  textures *textures;                           ///< Pointer.

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

  descriptor_binding uniformBufferBindings[VULKAN_UNIFORM_BUFFER_COUNT];
  descriptor_binding texturesBinding;

  /// Pointers to named uniform buffer binding.
#define decl_uniform_buffer(_name, ...) descriptor_binding *_name##UniformBufferBinding;
  VULKAN_UNIFORM_BUFFERS(decl_uniform_buffer, )
#undef decl_uniform_buffer

} descriptors;

descriptors *descriptors_create(device *vkd, unified_uniform_buffer *unifiedUniformBuffer,
                                textures *textures);

void descriptors_destroy(descriptors *descriptors);

void descriptors_update(descriptors *descriptors);

void descriptors_send_to_device(descriptors *descriptors);

void descriptors_record_bind_commands(descriptors *descriptors, VkCommandBuffer commandBuffer,
                                      draw_push_constant_element drawPushConstant);
VkDescriptorSetLayout *descriptors_get_descriptor_set_layouts(descriptors *descriptors,
                                                              size_t *count);

void descriptors_debug_print(descriptors *descriptors, int indent);
