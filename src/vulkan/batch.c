#include "batch.h"

vulkan_batch *vulkan_batch_create(vulkan_batch_policy policy, vulkan_scene_cache *firstCache) {
  assert(firstCache->primitive != NULL);

  vulkan_batch *batch = core_alloc(sizeof(vulkan_batch));
  batch->policy = policy;
  batch->primitive = firstCache->primitive;
  batch->first = 0;
  batch->count = 0;

  batch->prev = NULL;
  batch->next = NULL;
  return batch;
}

void vulkan_batch_destroy(vulkan_batch *batch) {
  // NOTE: Caches are destroyed by scene tree.
  core_free(batch);
}

bool vulkan_batch_matching_cache(vulkan_batch *batch, vulkan_scene_cache *cache) {
  bool materialMatch = (batch->policy & vulkan_batch_policy_matching_materials) == 0 ||
                       vulkan_data_primitive_material_match(batch->primitive, cache->primitive);
  bool geometryMatch =
      (batch->policy & vulkan_batch_policy_matching_vertex_attributes) == 0 ||
      vulkan_data_primitive_vulkan_attributes_match(batch->primitive, cache->primitive);
  return materialMatch && geometryMatch;
}

void vulkan_batch_debug_print(vulkan_batch *batch) {
  log_debug(
      "batch indexed indirect: policy=%d, first=%d count=%d materialHash=%zu geometryHash=%zu",
      batch->policy, batch->first, batch->count, batch->primitive->material->key.value,
      batch->primitive->geometryHash);
}

vulkan_batches *vulkan_batches_create(vulkan_scene_graph *sceneGraph) {
  vulkan_batches *batches = core_alloc(sizeof(vulkan_batches));

  batches->cacheList = sceneGraph->sceneTree->cacheList;
  batches->batches = NULL;

  return batches;
}

void vulkan_batches_destroy(vulkan_batches *batches) {
  vulkan_batches_reset(batches);
  core_free(batches);
}

void vulkan_batches_reset(vulkan_batches *batches) {
  dl_foreach_elem (vulkan_batch *, batch, batches->batches) { vulkan_batch_destroy(batch); }
  batches->batches = NULL;
}

void vulkan_batches_update(vulkan_batches *batches, vulkan_batch_policy policy) {

  vulkan_scene_cache_list_sort(batches->cacheList);

  vulkan_batches_reset(batches);

  assert(utarray_len(batches->cacheList->caches) > 0);
  vulkan_scene_cache *lastCache = NULL;
  vulkan_batch *lastBatch = NULL;

  utarray_foreach_elem_deref (vulkan_scene_cache *, cache, batches->cacheList->caches) {
    if (!cache->visible) {
      continue;
    }

    if (lastCache == NULL) {
      lastCache = cache;
    }

    if (lastBatch == NULL || !vulkan_batch_matching_cache(lastBatch, cache)) {
      vulkan_batch *newBatch = vulkan_batch_create(policy, cache);
      newBatch->first = 0;
      newBatch->count = 1;
      DL_APPEND(batches->batches, newBatch);
      lastCache = cache;
      lastBatch = newBatch;
    } else {
      lastBatch->count++;
    }
  }

  // HIRO: Consolidate smaller batches into one big batch with list of commands. (in scene render
  // context)?
}

void vulkan_batches_debug_print(vulkan_batches *batches) {
  log_debug("BATCHES:");
  dl_count(vulkan_batch *, batches->batches, batchLen);
  log_debug("batch count: %zu", batchLen);
  dl_foreach_elem (vulkan_batch *, batch, batches->batches) { vulkan_batch_debug_print(batch); }
}
