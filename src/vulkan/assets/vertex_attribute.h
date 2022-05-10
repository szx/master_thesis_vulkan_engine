/* Vertex attribute state.
 * Represents geometry data of a primitive.
 * Part of a primitive.
 */
#pragma once

#include "common.h"

typedef enum vulkan_asset_vertex_attribute_component_type {
  vulkan_asset_vertex_attribute_component_uint32_t,
  vulkan_asset_vertex_attribute_component_vec2,
  vulkan_asset_vertex_attribute_component_vec3
} vulkan_asset_vertex_attribute_component_type;

typedef struct vulkan_asset_vertex_attribute {
  vulkan_asset_vertex_attribute_component_type componentType;
  UT_array *data; /// uint32_t/vec2/vec3 array

  VULKAN_ASSET_FIELD_DECLS(vertex_attribute)
} vulkan_asset_vertex_attribute;

VULKAN_ASSET_FUNC_DECLS(vertex_attribute)
