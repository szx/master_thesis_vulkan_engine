/* Primitive state.
 * Represents part of a mesh.
 * Contains all information needed by a non-batched draw call.
 * Used by scene graph to prepare draw calls.
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"
#include "material.h"
#include "vertex_attribute.h"

typedef struct vulkan_data_scene vulkan_data_scene;

typedef struct vulkan_data_primitive {
  VkPrimitiveTopology topology;
  uint32_t vertexCount;
  vulkan_data_vertex_attribute *positions; /// vec3
  vulkan_data_vertex_attribute *normals;   /// vec3
  vulkan_data_vertex_attribute *colors;    /// vec3
  vulkan_data_vertex_attribute *texCoords; /// vec2
  // TODO: LOD index buffers.
  vulkan_data_vertex_attribute *indices; /// uint32_t
  hash_t geometryHash;

  vulkan_data_material *material;

  DECL_VULKAN_ENTITY(primitive)
} vulkan_data_primitive;

void vulkan_data_primitive_init(vulkan_data_primitive *primitive, vulkan_data_scene *sceneData);
void vulkan_data_primitive_deinit(vulkan_data_primitive *primitive);

data_key vulkan_data_primitive_calculate_key(vulkan_data_primitive *primitive);
void vulkan_data_primitive_serialize(vulkan_data_primitive *primitive, data_asset_db *assetDb);
void vulkan_data_primitive_deserialize(vulkan_data_primitive *primitive, data_asset_db *assetDb,
                                       data_key key);

bool vulkan_data_primitive_vulkan_attributes_match(vulkan_data_primitive *primitive,
                                                   vulkan_data_primitive *other);
bool vulkan_data_primitive_material_match(vulkan_data_primitive *primitive,
                                          vulkan_data_primitive *other);

void vulkan_data_primitive_debug_print(vulkan_data_primitive *primitive, int indent);
