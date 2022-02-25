/* GLSL shader generator */
#pragma once

#include "common.h"
#include "descriptor.h"
#include "render_state.h"
#include "shader_reflect.h"

/// Creates shaders used by pipeline.
typedef struct vulkan_shader_generator {
  // HIRO generate shaders from unified buffers.
  // HIRO generate *_uniform_descriptor_set in descriptor.h (layout + set)
  // HIRO match *_uniform_descriptor_set with shader reflect.
  // HIRO vulkan_shader_dependencies? and create render pass using it
  // HIRO: create descriptor set layouts from shader reflect

  /// Vertex shader.
  /// Transforms vertex input before rasterization.
  vulkan_shader *vertexShader;

  /// Fragment shader.
  vulkan_shader *fragmentShader;

  /// VkDescriptorSetLayout array. One descriptor set layout per one shader binding (layout
  /// qualifier). VK_NULL_HANDLE if no shader binding for indexed set number.
  UT_array *descriptorSetLayouts;

  // HIRO generate descriptor sets from shader generator layouts.
  VkDescriptorPool descriptorPool;
  /// One descriptor set per one descriptor set layout.
  VkDescriptorSet descriptorSet;
} vulkan_shader_generator;

vulkan_shader_generator *vulkan_shader_generator_create(vulkan_render_state *renderState);
void vulkan_shader_generator_destroy(vulkan_shader_generator *shaderGenerator);

void vulkan_shader_generator_debug_print(vulkan_shader_generator *shaderGenerator, int indent);
