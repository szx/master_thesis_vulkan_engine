/* Vulkan scene cache.
 * Represents scene node state that has been accumulated while traversing
 * from scene node to its successors.
 * Used to:
 * accumulate scene node state changes propagated from dirty scene node,
 * build render data from primitive scene nodes.
 */
#pragma once

#include "../data/scene.h"

typedef struct vulkan_scene_node vulkan_scene_node;

typedef struct vulkan_scene_cache {
  vulkan_scene_node *node;

  /* cache state accumulated from scene tree */
  size_t distanceFromRoot;
  bool visible;
  mat4 transform;                   /// Accumulated from object node.
  vulkan_data_mesh *mesh;           /// Accumulated from object node.
  vulkan_data_primitive *primitive; /// Accumulated from primitive node.

  /* cache state accumulated from vertex stream */
  size_t firstIndexOffset;
  size_t firstVertexOffset;

  /* cache state accumulated from batches */
  size_t cacheListIdx;

  struct vulkan_scene_cache *prev, *next;
} vulkan_scene_cache;

vulkan_scene_cache *vulkan_scene_cache_create(vulkan_scene_node *sceneNode);
void vulkan_scene_cache_destroy(vulkan_scene_cache *sceneCache);

/// Set cache with parent node.
void vulkan_scene_cache_set_with_node(vulkan_scene_cache *sceneCache);
void vulkan_scene_cache_accumulate(vulkan_scene_cache *sceneCache, vulkan_scene_cache *parentCache);

void vulkan_scene_cache_set_vertex_stream_offsets(vulkan_scene_cache *sceneCache,
                                                  size_t firstIndexOffset,
                                                  size_t firstVertexOffset);

void vulkan_scene_cache_debug_print(vulkan_scene_cache *sceneCache);
