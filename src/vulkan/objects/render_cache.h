/* Vulkan render cache.
 * Represents scene node state that has been accumulated while traversing
 * from scene node to its successors.
 * Used to:
 * accumulate scene node state changes propagated from dirty scene node,
 * build render data from primitive scene nodes.
 */
#pragma once

#include "../common.h"
#include "../data/camera.h"

typedef struct vulkan_scene_tree_node vulkan_scene_tree_node;
typedef struct vulkan_data_mesh vulkan_data_mesh;
typedef struct vulkan_data_primitive vulkan_data_primitive;

typedef struct vulkan_render_cache {
  vulkan_scene_tree_node *node;

  /* cache state accumulated from scene tree */
  size_t distanceFromRoot;
  bool visible;
  mat4 transform;                   ///< Accumulated from object node.
  vulkan_data_mesh *mesh;           ///< Accumulated from object node.
  vulkan_data_camera camera;        ///< Accumulated from object node.
  vulkan_data_primitive *primitive; ///< Accumulated from primitive node.

  /* cache state accumulated from vertex stream */
  size_t firstIndexOffset;
  size_t firstVertexOffset;

  /* cache state accumulated from batches */
  size_t renderCacheListIdx; ///< Also instanceId.

  struct vulkan_render_cache *prev, *next;
} vulkan_render_cache;

vulkan_render_cache *vulkan_render_cache_create(vulkan_scene_tree_node *sceneTreeNode);
void vulkan_render_cache_destroy(vulkan_render_cache *renderCache);

/// Set cache with parent node.
void vulkan_render_cache_set_with_node(vulkan_render_cache *renderCache);
void vulkan_render_cache_accumulate(vulkan_render_cache *renderCache,
                                    vulkan_render_cache *parentCache);

void vulkan_render_cache_set_vertex_stream_offsets(vulkan_render_cache *renderCache,
                                                   size_t firstIndexOffset,
                                                   size_t firstVertexOffset);

void vulkan_render_cache_debug_print(vulkan_render_cache *renderCache);
