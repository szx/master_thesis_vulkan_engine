#include "batch.h"
#include "../assets/primitive.h"

vulkan_draw_call *vulkan_draw_call_create(vulkan_draw_call_instancing_policy policy,
                                          vulkan_renderer_cache_primitive_element *firstCache) {
  assert(firstCache->primitive != NULL);

  vulkan_draw_call *batch = core_alloc(sizeof(vulkan_draw_call));
  batch->policy = policy;
  batch->firstCache = firstCache;

  batch->drawCommand.instanceCount = 0;
  vulkan_draw_call_update_draw_command(batch);
  batch->drawCommand.vertexOffset = INT32_MAX;
  batch->drawCommand.firstIndex = INT32_MAX;

  batch->prev = NULL;
  batch->next = NULL;
  return batch;
}

void vulkan_draw_call_destroy(vulkan_draw_call *batch) {
  // NOTE: Caches are destroyed by scene tree.
  core_free(batch);
}

bool vulkan_draw_call_matching_cache(vulkan_draw_call *batch,
                                     vulkan_renderer_cache_primitive_element *cache) {
  if (batch->policy == vulkan_draw_call_instancing_policy_matching_vertex_attributes) {
    return vulkan_asset_primitive_vulkan_attributes_match(batch->firstCache->primitive,
                                                          cache->primitive);
  }
  return false;
}

void vulkan_draw_call_add_cache(vulkan_draw_call *batch,
                                vulkan_renderer_cache_primitive_element *cache, size_t instanceId) {
  cache->instanceId = instanceId;
  batch->drawCommand.instanceCount++;
}

void vulkan_draw_call_update_draw_command(vulkan_draw_call *batch) {
  batch->drawCommand.indexCount = utarray_len(batch->firstCache->primitive->indices->data);
  batch->drawCommand.firstIndex = batch->firstCache->vertexStreamElement.firstIndexOffset;
  batch->drawCommand.vertexOffset = batch->firstCache->vertexStreamElement.firstVertexOffset;
  batch->drawCommand.firstInstance = batch->firstCache->instanceId;
}

void vulkan_draw_call_debug_print(vulkan_draw_call *batch) {
  log_debug("batch indexed indirect: policy=%d, indexCount=%d firstIndex=%d vertexOffset=%d "
            "firstInstance=%d instanceCount=%d materialHash=%zu "
            "geometryHash=%zu",
            batch->policy, batch->drawCommand.indexCount, batch->drawCommand.firstIndex,
            batch->drawCommand.vertexOffset, batch->drawCommand.firstInstance,
            batch->drawCommand.instanceCount, batch->firstCache->primitive->material->key.value,
            batch->firstCache->primitive->geometryHash);
}

void vulkan_draw_call_data_init(vulkan_draw_call_data *batchesData, vulkan_device *vkd) {

  batchesData->indirectDrawBuffer = vulkan_buffer_create(vkd, vulkan_buffer_type_indirect_draw);
  batchesData->indirectDrawBufferElement =
      vulkan_buffer_add(batchesData->indirectDrawBuffer, NULL,
                        sizeof(VkDrawIndexedIndirectCommand) * vkd->limits.maxDrawIndirectCommands);
}

void vulkan_draw_call_data_deinit(vulkan_draw_call_data *batchesData) {
  vulkan_buffer_destroy(batchesData->indirectDrawBuffer);
}

void vulkan_draw_call_data_send_to_device(vulkan_draw_call_data *batchesData) {
  vulkan_buffer_send_to_device(batchesData->indirectDrawBuffer);
}

vulkan_batches *vulkan_batches_create(vulkan_renderer_cache *rendererCache, vulkan_device *vkd) {
  vulkan_batches *batches = core_alloc(sizeof(vulkan_batches));

  batches->rendererCache = rendererCache;
  batches->vkd = vkd;
  batches->batches = NULL;

  return batches;
}

void vulkan_batches_destroy(vulkan_batches *batches) {
  vulkan_batches_reset(batches);
  core_free(batches);
}

void vulkan_batches_reset(vulkan_batches *batches) {
  dl_foreach_elem(vulkan_draw_call *, batch, batches->batches) { vulkan_draw_call_destroy(batch); }
  batches->batches = NULL;
}

void vulkan_batches_update(vulkan_batches *batches, vulkan_draw_call_instancing_policy policy) {
  // sort renderer cache elements and update attributes
  // HIRO Refactor do not sort directly in renderer cache, maintain separate list
  vulkan_renderer_cache_sort_primitive_elements(batches->rendererCache);
  assert(batches->rendererCache->primitiveElementsSorted);

  vulkan_batches_reset(batches);

  assert(dl_count(batches->rendererCache->primitiveElements) > 0);

  vulkan_renderer_cache_primitive_element *lastPrimitiveElement = NULL;
  vulkan_draw_call *lastBatch = NULL;
  size_t instanceId = 0;

  // HIRO refactor batches to separate list of primitive elements
  dl_foreach_elem(vulkan_renderer_cache_primitive_element *, primitiveElement,
                  batches->rendererCache->primitiveElements) {
    if (!primitiveElement->visible) {
      continue;
    }

    if (lastPrimitiveElement == NULL) {
      lastPrimitiveElement = primitiveElement;
    }

    if (lastBatch == NULL || !vulkan_draw_call_matching_cache(lastBatch, primitiveElement)) {
      vulkan_draw_call *newBatch = vulkan_draw_call_create(policy, primitiveElement);
      vulkan_draw_call_add_cache(newBatch, primitiveElement, instanceId);

      DL_APPEND(batches->batches, newBatch);
      lastPrimitiveElement = primitiveElement;
      lastBatch = newBatch;
    } else {
      vulkan_draw_call_add_cache(lastBatch, primitiveElement, instanceId);
    }

    instanceId++;
  }
}

void vulkan_batches_record_draw_command(vulkan_batches *batches, VkCommandBuffer commandBuffer,
                                        vulkan_draw_call_data *batchesData) {
  size_t drawCommandCount = dl_count(batches->batches);
  verify(drawCommandCount <= batches->vkd->limits.maxDrawIndirectCommands);
  if (drawCommandCount == 0) {
    return;
  }
  VkDrawIndexedIndirectCommand drawCommands[drawCommandCount];
  size_t drawCommandIdx = 0;
  dl_foreach_elem(vulkan_draw_call *, batch, batches->batches) {
    drawCommands[drawCommandIdx++] = batch->drawCommand;
  }

  batchesData->indirectDrawBufferElement.data = drawCommands;
  batchesData->indirectDrawBufferElement.size = sizeof(drawCommands);
  vulkan_buffer_element_update(&batchesData->indirectDrawBufferElement);

  size_t drawCommandStride = sizeof(drawCommands[0]);
  vkCmdDrawIndexedIndirect(commandBuffer, batchesData->indirectDrawBufferElement.buffer->buffer, 0,
                           drawCommandCount, drawCommandStride);
}

void vulkan_batches_debug_print(vulkan_batches *batches) {
  log_debug("BATCHES:");
  log_debug("batch count: %zu", dl_count(batches->batches));
  dl_foreach_elem(vulkan_draw_call *, batch, batches->batches) {
    vulkan_draw_call_debug_print(batch);
  }
}
