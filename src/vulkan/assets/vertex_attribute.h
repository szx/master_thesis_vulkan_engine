/* Vertex attribute state.
 * Represents geometry data of a primitive.
 * Part of a primitive.
 */
#pragma once

#include "common.h"

typedef enum asset_vertex_attribute_component_type {
  asset_vertex_attribute_component_uint32_t,
  asset_vertex_attribute_component_vec2,
  asset_vertex_attribute_component_vec3,
  asset_vertex_attribute_component_vec4,
  asset_vertex_attribute_component_count,
} asset_vertex_attribute_component_type;

typedef struct asset_vertex_attribute {
  asset_vertex_attribute_component_type componentType;
  UT_array *data; /// uint32_t/vec2/vec3 array

  VULKAN_ASSET_FIELD_DECLS(vertex_attribute)
} asset_vertex_attribute;

VULKAN_ASSET_FUNC_DECLS(vertex_attribute)
