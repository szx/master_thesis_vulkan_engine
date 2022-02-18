#include "batch.h"

vulkan_batch *vulkan_batch_create(vulkan_batch_policy policy, vulkan_scene_cache *firstCache) {
  assert(firstCache->primitive != NULL);

  vulkan_batch *batch = core_alloc(sizeof(vulkan_batch));
  batch->policy = policy;
  batch->firstCache = firstCache;

  batch->drawCommand.instanceCount = 0;
  vulkan_batch_update_draw_command(batch);
  batch->drawCommand.vertexOffset = INT32_MAX;
  batch->drawCommand.firstIndex = INT32_MAX;

  batch->prev = NULL;
  batch->next = NULL;
  return batch;
}

void vulkan_batch_destroy(vulkan_batch *batch) {
  // NOTE: Caches are destroyed by scene tree.
  core_free(batch);
}

bool vulkan_batch_matching_cache(vulkan_batch *batch, vulkan_scene_cache *cache) {
  bool match = true;
  if ((batch->policy & vulkan_batch_policy_matching_materials) != 0) {
    match = match &&
            vulkan_data_primitive_material_match(batch->firstCache->primitive, cache->primitive);
  }
  if ((batch->policy & vulkan_batch_policy_matching_vertex_attributes) != 0) {
    match = match && vulkan_data_primitive_vulkan_attributes_match(batch->firstCache->primitive,
                                                                   cache->primitive);
  }
  return match;
}

void vulkan_batch_add_cache(vulkan_batch *batch, vulkan_scene_cache *cache, size_t cacheListIdx) {
  cache->cacheListIdx = cacheListIdx;
  batch->drawCommand.instanceCount++;
}

void vulkan_batch_update_draw_command(vulkan_batch *batch) {
  batch->drawCommand.indexCount = utarray_len(batch->firstCache->primitive->indices->data);
  batch->drawCommand.firstIndex = batch->firstCache->firstIndexOffset;
  batch->drawCommand.vertexOffset = batch->firstCache->firstVertexByteOffset;
  batch->drawCommand.firstInstance = batch->firstCache->cacheListIdx;
}

void vulkan_batch_debug_print(vulkan_batch *batch) {
  log_debug("batch indexed indirect: policy=%d, indexCount=%d firstIndex=%d vertexOffset=%d "
            "firstInstance=%d instanceCount=%d materialHash=%zu "
            "geometryHash=%zu",
            batch->policy, batch->drawCommand.indexCount, batch->drawCommand.firstIndex,
            batch->drawCommand.vertexOffset, batch->drawCommand.firstInstance,
            batch->drawCommand.instanceCount, batch->firstCache->primitive->material->key.value,
            batch->firstCache->primitive->geometryHash);
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
  dl_foreach_elem(vulkan_batch *, batch, batches->batches, { vulkan_batch_destroy(batch); })
  batches->batches = NULL;
}

void vulkan_batches_update(vulkan_batches *batches, vulkan_batch_policy policy) {
  // sort cache list and update attributes
  vulkan_scene_cache_list_sort_and_update(batches->cacheList);

  vulkan_batches_reset(batches);

  assert(utarray_len(batches->cacheList->caches) > 0);
  vulkan_scene_cache *lastCache = NULL;
  vulkan_batch *lastBatch = NULL;
  size_t cacheListIdx = 0;
  utarray_foreach_elem_deref (vulkan_scene_cache *, cache, batches->cacheList->caches) {
    if (!cache->visible) {
      continue;
    }

    if (lastCache == NULL) {
      lastCache = cache;
    }

    if (lastBatch == NULL || !vulkan_batch_matching_cache(lastBatch, cache)) {
      vulkan_batch *newBatch = vulkan_batch_create(policy, cache);
      vulkan_batch_add_cache(newBatch, cache, cacheListIdx);

      DL_APPEND(batches->batches, newBatch);
      lastCache = cache;
      lastBatch = newBatch;
    } else {
      vulkan_batch_add_cache(lastBatch, cache, cacheListIdx);
    }

    cacheListIdx++;
  }
  // HIRO: Consolidate smaller batches into one big batch with list of commands. (in scene render
  // context)?
}

void vulkan_batches_debug_print(vulkan_batches *batches) {
  log_debug("BATCHES:");
  dl_count(vulkan_batch *, batches->batches, batchLen);
  log_debug("batch count: %zu", batchLen);
  dl_foreach_elem(vulkan_batch *, batch, batches->batches, { vulkan_batch_debug_print(batch); })
}
