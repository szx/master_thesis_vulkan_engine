/* Primitive state.
 * Represents part of a mesh.
 * Contains all information needed by a non-batched draw call.
 * Used by scene graph to prepare draw calls.
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"

/// Contains index buffer, interleaved vertex stream and topology of the part of the mesh.
typedef struct vulkan_primitive_data {
  VkPrimitiveTopology topology;
  uint32_t vertexCount;
  UT_array *positions; /// vec3
  UT_array *normals;   /// vec3
  UT_array *colors;    /// vec3
  UT_array *texCoords; /// vec2
  // TODO: LOD index buffers.
  UT_array *indices; /// uint32_t
  // TODO: Material.
  data_key hash; /// Hash, used to prevent duplicates in asset database.
} vulkan_primitive_data;

void vulkan_primitive_data_init(vulkan_primitive_data *primitive);
void vulkan_primitive_data_deinit(vulkan_primitive_data *primitive);

data_key vulkan_primitive_data_calculate_key(vulkan_primitive_data *primitive);
void vulkan_primitive_data_serialize(vulkan_primitive_data *primitive, data_asset_db *assetDb);
void vulkan_primitive_data_deserialize(vulkan_primitive_data *primitive, data_asset_db *assetDb,
                                       data_key key);

void vulkan_primitive_data_debug_print(vulkan_primitive_data *primitive);
