/* Vulkan draw call batches.
 * Tracks primitive nodes in scene.
 * Used to generate Vulkan draw commands.
 */
#pragma once

#include "../core/core.h"
#include "scene/graph.h"

/// Each flag imposes constraint on a batch, which results in smaller batches and more draw calls.
typedef enum vulkan_batch_flags {
  vulkan_batch_none_flag = 0,                     /// No constraints, ideal one big batch.
  vulkan_batch_vertex_attributes_flag = (1 << 0), /// Same vertex attributes.
  vulkan_batch_material_flag = (1 << 1),          /// Same material.
  vulkan_batch_vertex_all_flag = vulkan_batch_vertex_attributes_flag | vulkan_batch_material_flag,
} vulkan_batch_flags;

/// Draw call batch.
typedef struct vulkan_batch {
  vulkan_batch_flags flags;
  vulkan_scene_cache *caches; /// List of vulkan_scene_cache
  // HIRO vulkan draw call data

  struct vulkan_batch *prev, *next; /// Linked list.
} vulkan_batch;

vulkan_batch *vulkan_batch_create(vulkan_batch_flags flags);
void vulkan_batch_destroy(vulkan_batch *batch);

bool vulkan_batch_matches_cache(vulkan_batch *batch, vulkan_scene_cache *cache);
void vulkan_batch_add_cache(vulkan_batch *batch, vulkan_scene_cache *cache);
void vulkan_batch_debug_print(vulkan_batch *batch);

/// Creates batches from scene graph.
typedef struct vulkan_batches {
  vulkan_scene_graph *sceneGraph; /// Pointer.

  vulkan_batch *batches; /// List of batches created from scene graph.
} vulkan_batches;

vulkan_batches *vulkan_batches_create(vulkan_scene_graph *sceneGraph);
void vulkan_batches_destroy(vulkan_batches *batches);

/// Create and destroy batches using scene graph.
void vulkan_batches_update(vulkan_batches *batches);

// HIRO: Generate Vulkan draw commands.

void vulkan_batches_debug_print(vulkan_batches *batches);
