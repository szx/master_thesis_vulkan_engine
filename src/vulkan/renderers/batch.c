#include "batch.h"
#include "../assets/primitive.h"

void vulkan_batch_init(vulkan_batch *batch,
                       vulkan_renderer_cache_primitive_element *firstPrimitiveElement) {
  assert(firstPrimitiveElement->primitive != NULL);

  batch->firstPrimitiveElement = firstPrimitiveElement;

  batch->drawCommand.instanceCount = 0;
  vulkan_batch_update_draw_command(batch);
  batch->drawCommand.vertexOffset = INT32_MAX;
  batch->drawCommand.firstIndex = INT32_MAX;
}

void vulkan_batch_destroy(vulkan_batch *batch) {
  // No-op.
}

bool vulkan_batch_matching_primitive_element(vulkan_batch *batch,
                                             vulkan_renderer_cache_primitive_element *cache,
                                             vulkan_batch_instancing_policy policy) {
  if (policy == vulkan_batch_instancing_policy_matching_vertex_attributes) {
    return vulkan_asset_primitive_vulkan_attributes_match(batch->firstPrimitiveElement->primitive,
                                                          cache->primitive);
  }
  return false;
}

void vulkan_batch_add_primitive_element(vulkan_batch *batch,
                                        vulkan_renderer_cache_primitive_element *cache,
                                        size_t instanceId) {
  cache->instanceId = instanceId;
  batch->drawCommand.instanceCount++;
}

void vulkan_batch_update_draw_command(vulkan_batch *batch) {
  batch->drawCommand.indexCount =
      utarray_len(batch->firstPrimitiveElement->primitive->indices->data);
  batch->drawCommand.firstIndex =
      batch->firstPrimitiveElement->vertexStreamElement.firstIndexOffset;
  batch->drawCommand.vertexOffset =
      batch->firstPrimitiveElement->vertexStreamElement.firstVertexOffset;
  batch->drawCommand.firstInstance = batch->firstPrimitiveElement->instanceId;
}

void vulkan_batch_debug_print(vulkan_batch *batch) {
  log_debug("batch indexed indirect: indexCount=%d firstIndex=%d vertexOffset=%d "
            "firstInstance=%d instanceCount=%d materialHash=%zu "
            "geometryHash=%zu",
            batch->drawCommand.indexCount, batch->drawCommand.firstIndex,
            batch->drawCommand.vertexOffset, batch->drawCommand.firstInstance,
            batch->drawCommand.instanceCount,
            batch->firstPrimitiveElement->primitive->material->key.value,
            batch->firstPrimitiveElement->primitive->geometryHash);
}

void vulkan_batches_buffer_init(vulkan_batches_buffer *batchesData, vulkan_device *vkd) {

  batchesData->indirectDrawBuffer = vulkan_buffer_create(vkd, vulkan_buffer_type_indirect_draw);
  batchesData->indirectDrawBufferElement =
      vulkan_buffer_add(batchesData->indirectDrawBuffer, NULL,
                        sizeof(VkDrawIndexedIndirectCommand) * vkd->limits.maxDrawIndirectCommands);
}

void vulkan_batches_buffer_deinit(vulkan_batches_buffer *batchesData) {
  vulkan_buffer_destroy(batchesData->indirectDrawBuffer);
}

void vulkan_batches_buffer_send_to_device(vulkan_batches_buffer *batchesData) {
  vulkan_buffer_send_to_device(batchesData->indirectDrawBuffer);
}

vulkan_batches *vulkan_batches_create(vulkan_device *vkd) {
  vulkan_batches *batches = core_alloc(sizeof(vulkan_batches));

  batches->vkd = vkd;
  utarray_alloc(batches->primitiveElements, sizeof(vulkan_renderer_cache_primitive_element *));
  utarray_alloc(batches->array, sizeof(vulkan_batch));

  batches->_primitiveElementsDirty = false;

  return batches;
}

void vulkan_batches_destroy(vulkan_batches *batches) {
  utarray_free(batches->primitiveElements);

  vulkan_batches_reset(batches);
  utarray_free(batches->array);

  core_free(batches);
}

void vulkan_batches_reset(vulkan_batches *batches) {
  utarray_foreach_elem_it (vulkan_batch *, batch, batches->array) { vulkan_batch_destroy(batch); }
}

void vulkan_batches_add_primitive_element(
    vulkan_batches *batches, vulkan_renderer_cache_primitive_element *primitiveElement) {
  log_debug("adding primitive element to batches");

  utarray_push_back(batches->primitiveElements, &primitiveElement);
  batches->_primitiveElementsDirty = true;
}

static int sort_primitive_elements_before_update_cmp(const void *aPtr, const void *bPtr) {
  const vulkan_renderer_cache_primitive_element *a =
      *(const vulkan_renderer_cache_primitive_element **)aPtr;
  const vulkan_renderer_cache_primitive_element *b =
      *(const vulkan_renderer_cache_primitive_element **)bPtr;

  // sort by visibility
  if (a->visible && !b->visible) {
    return -1;
  }
  if (!a->visible && b->visible) {
    return 1;
  }
  if (!a->visible && !b->visible) {
    return 0; // Both invisible, do not care.
  }

  if (a->primitive == b->primitive) {
    return 0; // Same geometry and material, ideal for batching.
  }

  // sort by geometry
  if (a->primitive->geometryHash < b->primitive->geometryHash) {
    return -1;
  }
  if (a->primitive->geometryHash > b->primitive->geometryHash) {
    return 1;
  }
  return 0;
}

static void sort_primitive_elements_before_update(vulkan_batches *batches) {
  log_debug("sorting primitive elements before batches update");
  // PERF: Sorting primitives should depend on selected instancing policy.
  assert(utarray_len(batches->primitiveElements) > 0);

  // sort primitive elements in order to minimize number of instances
  utarray_sort(batches->primitiveElements, sort_primitive_elements_before_update_cmp);
}

void vulkan_batches_update(vulkan_batches *batches, vulkan_batch_instancing_policy policy) {
  if (!batches->_primitiveElementsDirty) {
    return;
  }

  // sort renderer cache elements and update attributes
  sort_primitive_elements_before_update(batches);
  assert(utarray_len(batches->primitiveElements) > 0);

  log_debug("updating batches");
  vulkan_batches_reset(batches);

  vulkan_renderer_cache_primitive_element *lastPrimitiveElement = NULL;
  vulkan_batch *lastBatch = NULL;
  size_t instanceId = 0;

  utarray_foreach_elem_deref (vulkan_renderer_cache_primitive_element *, primitiveElement,
                              batches->primitiveElements) {
    if (!primitiveElement->visible) {
      continue;
    }

    if (lastPrimitiveElement == NULL) {
      lastPrimitiveElement = primitiveElement;
    }

    if (lastBatch == NULL ||
        !vulkan_batch_matching_primitive_element(lastBatch, primitiveElement, policy)) {
      vulkan_batch newBatch;
      vulkan_batch_init(&newBatch, primitiveElement);
      vulkan_batch_add_primitive_element(&newBatch, primitiveElement, instanceId);

      utarray_push_back(batches->array, &newBatch);
      lastPrimitiveElement = primitiveElement;
      lastBatch = utarray_back(batches->array);
    } else {
      vulkan_batch_add_primitive_element(lastBatch, primitiveElement, instanceId);
    }

    instanceId++;
  }

  batches->_primitiveElementsDirty = false;
}

void vulkan_batches_record_draw_command(vulkan_batches *batches, VkCommandBuffer commandBuffer,
                                        vulkan_batches_buffer *batchesData) {

  size_t drawCommandCount = utarray_len(batches->array);
  verify(drawCommandCount <= batches->vkd->limits.maxDrawIndirectCommands);
  if (drawCommandCount == 0) {
    return;
  }

  VkDrawIndexedIndirectCommand drawCommands[drawCommandCount];
  size_t drawCommandIdx = 0;
  utarray_foreach_elem_it (vulkan_batch *, batch, batches->array) {
    vulkan_batch_update_draw_command(batch);
    assert(batch->drawCommand.firstIndex != INT32_MAX);
    assert(batch->drawCommand.vertexOffset != INT32_MAX);

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
  log_debug("batch count: %zu", utarray_len(batches->array));
  utarray_foreach_elem_it (vulkan_batch *, batch, batches->array) {
    vulkan_batch_debug_print(batch);
  }
}
