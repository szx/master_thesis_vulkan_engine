#include "batch.h"

vulkan_batch *vulkan_batch_create(vulkan_batch_flags flags) {
  vulkan_batch *batch = core_alloc(sizeof(vulkan_batch));
  batch->flags = flags;
  batch->caches = NULL;
  batch->prev = NULL;
  batch->next = NULL;
  return batch;
}

void vulkan_batch_destroy(vulkan_batch *batch) {
  // NOTE: Caches are destroyed by scene tree.
  core_free(batch);
}

bool vulkan_batch_matches_cache(vulkan_batch *batch, vulkan_scene_cache *cache) {
  assert(cache->primitive != NULL);

  if (batch->caches == NULL) {
    return true;
  }

  vulkan_scene_cache *batchedNode = batch->caches;
  bool matched = true;
  if ((batch->flags & vulkan_batch_vertex_attributes_flag) != 0) {
    if (!vulkan_data_primitive_vulkan_attributes_match(cache->primitive, batchedNode->primitive)) {
      matched = false;
    }
  } else if ((batch->flags & vulkan_batch_material_flag) != 0) {
    if (!vulkan_data_primitive_material_match(cache->primitive, batchedNode->primitive)) {
      matched = false;
    }
  }

  return matched;
}

void vulkan_batch_add_cache(vulkan_batch *batch, vulkan_scene_cache *cache) {
  assert(vulkan_batch_matches_cache(batch, cache));
  DL_APPEND(batch->caches, cache); // HIRO sort caches
}

void vulkan_batch_debug_print(vulkan_batch *batch) {
  log_raw(stdout, "digraph batch_%s {", vulkan_batch_flags_debug_str(batch->flags));
  dl_foreach_elem (vulkan_scene_cache *, cache, batch->caches) {
    vulkan_scene_cache_debug_print(cache);
  }
  log_raw(stdout, "}\n");
}

vulkan_batches *vulkan_batches_create(vulkan_scene_graph *sceneGraph) {
  vulkan_batches *batches = core_alloc(sizeof(vulkan_batches));

  batches->sceneGraph = sceneGraph;
  batches->batches = NULL;

  return batches;
}

void vulkan_batches_destroy(vulkan_batches *batches) {
  dl_foreach_elem (vulkan_batch *, batch, batches->batches) { vulkan_batch_destroy(batch); }
  core_free(batches);
}

void vulkan_batches_update(vulkan_batches *batches) {
  // HIRO: Sort all caches in cache_list by:
  // - visibility
  // - primitive material
  // - primitive vertex attributes
  // New batch if any of above changes.
  // One big batch for all caches.
  // Remove old batches.
  bool foundBatch = false;
  dl_foreach_elem (vulkan_batch *, batch, batches->batches) {
    if (vulkan_batch_matches_cache(batch, cache)) {
      vulkan_batch_add_cache(batch, cache);
      foundBatch = true;
      break;
    }
  }
  if (!foundBatch) {
    vulkan_batch *newBatch = vulkan_batch_create(vulkan_batch_vertex_all_flag);
    DL_APPEND(batches->batches, newBatch);
    vulkan_batch_add_cache(newBatch, cache);
  }
}

void vulkan_batches_debug_print(vulkan_batches *batches) {
  log_debug("BATCHES:");
  dl_foreach_elem (vulkan_batch *, batch, batches->batches) { vulkan_batch_debug_print(batch); }
}
