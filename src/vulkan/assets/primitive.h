/* Primitive state.
 * Represents part of a mesh.
 * Contains all information needed by a non-batched draw call.
 * Used by scene graph to prepare draw calls.
 */
#pragma once

#include "common.h"
#include "material.h"
#include "vertex_attribute.h"

typedef struct asset_primitive {
  VkPrimitiveTopology topology;
  uint32_t vertexCount;
  asset_vertex_attribute *positions; ///< vec3
  asset_vertex_attribute *normals;   ///< vec3
  asset_vertex_attribute *colors;    ///< vec3
  asset_vertex_attribute *texCoords; ///< vec2
  asset_vertex_attribute *tangents;  ///< vec4
  // TODO: LOD index buffers.
  asset_vertex_attribute *indices; ///< uint32_t
  hash_t geometryHash;
  vertex_attribute_type attributes;

  asset_material *material;

  VULKAN_ASSET_FIELD_DECLS(primitive)
} asset_primitive;

VULKAN_ASSET_FUNC_DECLS(primitive)

asset_primitive *
asset_primitive_create_from_geometry(scene_data *sceneData,
                                            VkPrimitiveTopology topology, uint32_t vertexCount,
                                            uint32_t *indices, vec3 *positions, vec3 *normals,
                                            vec3 *colors, vec2 *texCoords, vec4 *tangents);

bool asset_primitive_vertex_attributes_match(asset_primitive *primitive,
                                                    asset_primitive *other);

aabb asset_primitive_calculate_aabb(asset_primitive *primitive);
