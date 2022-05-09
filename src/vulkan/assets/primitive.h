/* Primitive state.
 * Represents part of a mesh.
 * Contains all information needed by a non-batched draw call.
 * Used by scene graph to prepare draw calls.
 */
#pragma once

#include "../functions.h"
#include "common.h"
#include "material.h"
#include "vertex_attribute.h"

typedef struct vulkan_asset_primitive {
  VkPrimitiveTopology topology;
  uint32_t vertexCount;
  vulkan_asset_vertex_attribute *positions; /// vec3
  vulkan_asset_vertex_attribute *normals;   /// vec3
  vulkan_asset_vertex_attribute *colors;    /// vec3
  vulkan_asset_vertex_attribute *texCoords; /// vec2
  // TODO: LOD index buffers.
  vulkan_asset_vertex_attribute *indices; /// uint32_t
  hash_t geometryHash;
  vulkan_attribute_type attributes;

  vulkan_asset_material *material;

  VULKAN_ASSET_FIELD_DECLS(primitive)
} vulkan_asset_primitive;

VULKAN_ASSET_FUNC_DECLS(primitive)

bool vulkan_asset_primitive_vulkan_attributes_match(vulkan_asset_primitive *primitive,
                                                    vulkan_asset_primitive *other);

vulkan_aabb vulkan_asset_primitive_calculate_aabb(vulkan_asset_primitive *primitive);
