#include "batch.h"
#include "../data/primitive.h"

vulkan_batch *vulkan_batch_create(vulkan_batch_instancing_policy policy,
                                  vulkan_render_cache *firstCache) {
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

bool vulkan_batch_matching_cache(vulkan_batch *batch, vulkan_render_cache *cache) {
  if (batch->policy == vulkan_batch_instancing_policy_matching_vertex_attributes) {
    return vulkan_data_primitive_vulkan_attributes_match(batch->firstCache->primitive,
                                                         cache->primitive);
  }
  return false;
}

void vulkan_batch_add_cache(vulkan_batch *batch, vulkan_render_cache *cache, size_t instanceId) {
  cache->instanceId = instanceId;
  batch->drawCommand.instanceCount++;
}

void vulkan_batch_update_draw_command(vulkan_batch *batch) {
  batch->drawCommand.indexCount = utarray_len(batch->firstCache->primitive->indices->data);
  batch->drawCommand.firstIndex = batch->firstCache->firstIndexOffset;
  batch->drawCommand.vertexOffset = batch->firstCache->firstVertexOffset;
  batch->drawCommand.firstInstance = batch->firstCache->instanceId;
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

vulkan_batches *vulkan_batches_create(vulkan_render_cache_list *renderCacheList,
                                      vulkan_device *vkd) {
  vulkan_batches *batches = core_alloc(sizeof(vulkan_batches));

  batches->renderCacheList = renderCacheList;
  batches->vkd = vkd;
  batches->batches = NULL;
  batches->indirectDrawBuffer =
      vulkan_buffer_create(batches->vkd, vulkan_buffer_type_indirect_draw);
  batches->indirectDrawBufferElement = vulkan_buffer_add(
      batches->indirectDrawBuffer, NULL,
      sizeof(VkDrawIndexedIndirectCommand) * batches->vkd->limits.maxDrawIndirectCommands);
  return batches;
}

void vulkan_batches_destroy(vulkan_batches *batches) {
  vulkan_buffer_destroy(batches->indirectDrawBuffer);
  vulkan_batches_reset(batches);
  core_free(batches);
}

void vulkan_batches_reset(vulkan_batches *batches) {
  dl_foreach_elem(vulkan_batch *, batch, batches->batches) { vulkan_batch_destroy(batch); }
  batches->batches = NULL;
}

void vulkan_batches_update(vulkan_batches *batches, vulkan_batch_instancing_policy policy) {
  // sort cache list and update attributes
  vulkan_render_cache_list_update(batches->renderCacheList);
  assert(!batches->renderCacheList->dirty);

  vulkan_batches_reset(batches);

  assert(utarray_len(batches->renderCacheList->primitiveRenderCaches) > 0);
  vulkan_render_cache *lastCache = NULL;
  vulkan_batch *lastBatch = NULL;
  size_t instanceId = 0;
  utarray_foreach_elem_deref (vulkan_render_cache *, cache,
                              batches->renderCacheList->primitiveRenderCaches) {
    if (!cache->visible) {
      continue;
    }

    if (lastCache == NULL) {
      lastCache = cache;
    }

    if (lastBatch == NULL || !vulkan_batch_matching_cache(lastBatch, cache)) {
      vulkan_batch *newBatch = vulkan_batch_create(policy, cache);
      vulkan_batch_add_cache(newBatch, cache, instanceId);

      DL_APPEND(batches->batches, newBatch);
      lastCache = cache;
      lastBatch = newBatch;
    } else {
      vulkan_batch_add_cache(lastBatch, cache, instanceId);
    }

    instanceId++;
  }
}

void vulkan_batches_send_to_device(vulkan_batches *batches) {
  vulkan_buffer_send_to_device(batches->indirectDrawBuffer);
}

void vulkan_batches_record_draw_command(vulkan_batches *batches, VkCommandBuffer commandBuffer) {
  dl_count(vulkan_batch *, batches->batches, drawCommandCount);
  verify(drawCommandCount <= batches->vkd->limits.maxDrawIndirectCommands);
  if (drawCommandCount == 0) {
    return;
  }
  VkDrawIndexedIndirectCommand drawCommands[drawCommandCount];
  size_t drawCommandIdx = 0;
  dl_foreach_elem(vulkan_batch *, batch, batches->batches) {
    drawCommands[drawCommandIdx++] = batch->drawCommand;
  }

  batches->indirectDrawBufferElement.data = drawCommands;
  batches->indirectDrawBufferElement.size = sizeof(drawCommands);
  vulkan_buffer_element_update(&batches->indirectDrawBufferElement);

  size_t drawCommandStride = sizeof(drawCommands[0]);
  vkCmdDrawIndexedIndirect(commandBuffer, batches->indirectDrawBuffer->buffer, 0, drawCommandCount,
                           drawCommandStride);
}

void vulkan_batches_debug_print(vulkan_batches *batches) {
  log_debug("BATCHES:");
  dl_count(vulkan_batch *, batches->batches, batchLen);
  log_debug("batch count: %zu", batchLen);
  dl_foreach_elem(vulkan_batch *, batch, batches->batches) { vulkan_batch_debug_print(batch); }
}
