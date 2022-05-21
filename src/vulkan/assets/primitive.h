/* Primitive state.
 * Represents part of a mesh.
 * Contains all information needed by a non-batched draw call.
 * Used by scene graph to prepare draw calls.
 */
#pragma once

#include "common.h"
#include "material.h"
#include "vertex_attribute.h"

typedef struct vulkan_asset_primitive {
  VkPrimitiveTopology topology;
  uint32_t vertexCount;
  vulkan_asset_vertex_attribute *positions; ///< vec3
  vulkan_asset_vertex_attribute *normals;   ///< vec3
  vulkan_asset_vertex_attribute *colors;    ///< vec3
  vulkan_asset_vertex_attribute *texCoords; ///< vec2
  vulkan_asset_vertex_attribute *tangents;  ///< vec4
  // TODO: LOD index buffers.
  vulkan_asset_vertex_attribute *indices; ///< uint32_t
  hash_t geometryHash;
  vulkan_attribute_type attributes;

  vulkan_asset_material *material;

  VULKAN_ASSET_FIELD_DECLS(primitive)
} vulkan_asset_primitive;

VULKAN_ASSET_FUNC_DECLS(primitive)

vulkan_asset_primitive *
vulkan_asset_primitive_create_from_geometry(vulkan_scene_data *sceneData,
                                            VkPrimitiveTopology topology, uint32_t vertexCount,
                                            uint32_t *indices, vec3 *positions, vec3 *normals,
                                            vec3 *colors, vec2 *texCoords, vec4 *tangents);

bool vulkan_asset_primitive_vulkan_attributes_match(vulkan_asset_primitive *primitive,
                                                    vulkan_asset_primitive *other);

vulkan_aabb vulkan_asset_primitive_calculate_aabb(vulkan_asset_primitive *primitive);
