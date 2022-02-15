/* Primitive state.
 * Represents part of a mesh.
 * Contains all information needed by a non-batched draw call.
 * Used by scene graph to prepare draw calls.
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"
#include "material.h"

typedef struct vulkan_data_scene vulkan_data_scene;

typedef struct vulkan_data_primitive {
  VkPrimitiveTopology topology;
  uint32_t vertexCount;
  UT_array *positions; /// vec3
  UT_array *normals;   /// vec3
  UT_array *colors;    /// vec3
  UT_array *texCoords; /// vec2
  // TODO: LOD index buffers.
  UT_array *indices;   /// uint32_t
  hash_t geometryHash; // TODO: Split vertex streams.

  vulkan_data_material *material;

  DECL_VULKAN_ENTITY(primitive)
} vulkan_data_primitive;

void vulkan_data_primitive_init(vulkan_data_primitive *primitive, vulkan_data_scene *sceneData);
void vulkan_data_primitive_deinit(vulkan_data_primitive *primitive);

data_key vulkan_data_primitive_calculate_key(vulkan_data_primitive *primitive);
void vulkan_data_primitive_serialize(vulkan_data_primitive *primitive, data_asset_db *assetDb);
void vulkan_data_primitive_deserialize(vulkan_data_primitive *primitive, data_asset_db *assetDb,
                                       data_key key);

void vulkan_data_primitive_debug_print(vulkan_data_primitive *primitive, int indent);
