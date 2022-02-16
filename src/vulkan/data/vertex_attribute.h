/* Vertex attribute state.
 * Represents geometry data of a primitive.
 * Part of a primitive.
 */
#pragma once

#include "../functions.h"
#include "common.h"

typedef enum vulkan_data_vertex_attribute_component_type {
  vulkan_data_vertex_attribute_component_uint32_t,
  vulkan_data_vertex_attribute_component_vec2,
  vulkan_data_vertex_attribute_component_vec3
} vulkan_data_vertex_attribute_component_type;

typedef struct vulkan_data_vertex_attribute {
  vulkan_data_vertex_attribute_component_type componentType;
  UT_array *data; /// uint32_t/vec2/vec3 array

  DECL_VULKAN_ENTITY(vertex_attribute)
} vulkan_data_vertex_attribute;

void vulkan_data_vertex_attribute_init(vulkan_data_vertex_attribute *vertexAttribute,
                                       vulkan_data_scene *sceneData);
void vulkan_data_vertex_attribute_deinit(vulkan_data_vertex_attribute *vertexAttribute);

data_key vulkan_data_vertex_attribute_calculate_key(vulkan_data_vertex_attribute *vertexAttribute);
void vulkan_data_vertex_attribute_serialize(vulkan_data_vertex_attribute *vertexAttribute,
                                            data_asset_db *assetDb);
void vulkan_data_vertex_attribute_deserialize(vulkan_data_vertex_attribute *vertexAttribute,
                                              data_asset_db *assetDb, data_key key);

void vulkan_data_vertex_attribute_debug_print(vulkan_data_vertex_attribute *vertexAttribute,
                                              int indent);
