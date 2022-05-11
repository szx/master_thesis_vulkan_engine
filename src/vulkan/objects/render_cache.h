/* Vulkan render cache.
 * Represents scene node state that has been accumulated while traversing
 * from scene node to its successors.
 * Used to:
 * accumulate scene node state changes propagated from dirty scene node,
 * build render data from primitive scene nodes.
 */
#pragma once

#include "../assets/assets.h"
#include "../common.h"
#include "textures.h"

// HIRO Refactor remove typedef
typedef struct vulkan_textures_material_element vulkan_textures_material_element;

// HIRO refactor split into multiple types
// HIRO add key to prevent multiple adding in vulkan_render_cache_list
typedef struct vulkan_render_cache {
  /* cache state accumulated from scene tree */
  size_t distanceFromRoot;
  bool visible;
  mat4 transform;                    ///< Accumulated from object node.
  vulkan_asset_mesh *mesh;           ///< Accumulated from object node.
  vulkan_asset_camera camera;        ///< Accumulated from object node.
  vulkan_asset_primitive *primitive; ///< Accumulated from primitive node.
  vulkan_aabb aabb;                  ///< Accumulated from primitive node.

  /* cache state accumulated from vertex stream */
  size_t firstIndexOffset;
  size_t firstVertexOffset;

  /* cache state accumulated during draw call batching. */
  size_t instanceId; ///< Index in render cache list, equals gl_InstanceIndex in shader thanks to
                     ///< draw call batching.

  /* cache state accumulated from render cache list */
  vulkan_textures_material_element
      *materialElement; ///< Contains index of material in materials array in textures.

  struct vulkan_render_cache *prev, *next;
} vulkan_render_cache;

vulkan_render_cache *vulkan_render_cache_create();
void vulkan_render_cache_destroy(vulkan_render_cache *renderCache);

void vulkan_render_cache_reset(vulkan_render_cache *renderCache);

void vulkan_render_cache_set_vertex_stream_offsets(vulkan_render_cache *renderCache,
                                                   size_t firstIndexOffset,
                                                   size_t firstVertexOffset);

void vulkan_render_cache_debug_print(vulkan_render_cache *renderCache);
