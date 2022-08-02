#include "batch.h"
#include "../assets/primitive.h"

void batch_init(batch *batch, renderer_cache_primitive_element *firstPrimitiveElement,
                size_t firstInstanceId) {
  assert(firstPrimitiveElement->primitive != NULL);

  batch->firstPrimitiveElement = firstPrimitiveElement;

  batch->drawCommand.instanceCount = 0;
  batch->drawCommand = (VkDrawIndexedIndirectCommand){0};
  batch_add_primitive_element(batch, batch->firstPrimitiveElement, firstInstanceId);
}

void batch_deinit(batch *batch) {
  // No-op.
}

bool batch_matching_primitive_element(batch *batch, renderer_cache_primitive_element *cache,
                                      batch_instancing_policy policy) {
  if (policy == batch_instancing_policy_matching_vertex_attributes) {
    return asset_primitive_vertex_attributes_match(batch->firstPrimitiveElement->primitive,
                                                   cache->primitive);
  }
  return false;
}

void batch_add_primitive_element(batch *batch, renderer_cache_primitive_element *cache,
                                 size_t instanceId) {
  renderer_cache_primitive_set_instance_id(cache, instanceId);
  batch->drawCommand.instanceCount++;
}

void batch_update_draw_command(batch *batch) {
  assert(renderer_cache_primitive_is_valid(batch->firstPrimitiveElement, instanceId));
  assert(renderer_cache_primitive_is_valid(batch->firstPrimitiveElement, vertexStreamElement));

  batch->drawCommand.indexCount =
      utarray_len(batch->firstPrimitiveElement->primitive->indices->data);
  batch->drawCommand.firstIndex =
      batch->firstPrimitiveElement->vertexStreamElement.firstIndexOffset;
  batch->drawCommand.vertexOffset =
      batch->firstPrimitiveElement->vertexStreamElement.firstVertexOffset;
  batch->drawCommand.firstInstance = batch->firstPrimitiveElement->instanceId;
}

void batch_record_basic_draw_command(batch *batch, VkCommandBuffer commandBuffer) {
  batch_update_draw_command(batch);
  vkCmdDrawIndexed(commandBuffer, batch->drawCommand.indexCount, batch->drawCommand.instanceCount,
                   batch->drawCommand.firstIndex, batch->drawCommand.vertexOffset,
                   batch->drawCommand.firstInstance);
}

void batch_debug_print(batch *batch) {
  log_debug("batch indexed indirect: indexCount=%d firstIndex=%d vertexOffset=%d "
            "firstInstance=%d instanceCount=%d materialHash=%zu "
            "geometryHash=%zu",
            batch->drawCommand.indexCount, batch->drawCommand.firstIndex,
            batch->drawCommand.vertexOffset, batch->drawCommand.firstInstance,
            batch->drawCommand.instanceCount,
            batch->firstPrimitiveElement->primitive->material->key.value,
            batch->firstPrimitiveElement->primitive->geometryHash);
}

void batches_buffer_init(batches_buffer *batchesData, device *vkd) {

  batchesData->indirectDrawBuffer = buffer_create(vkd, buffer_type_indirect_draw);
  batchesData->indirectDrawBufferElement =
      buffer_add(batchesData->indirectDrawBuffer, NULL,
                 sizeof(VkDrawIndexedIndirectCommand) * vkd->limits.maxDrawIndirectCommands);
}

void batches_buffer_deinit(batches_buffer *batchesData) {
  buffer_destroy(batchesData->indirectDrawBuffer);
}

void batches_buffer_send_to_device(batches_buffer *batchesData) {
  buffer_send_to_device(batchesData->indirectDrawBuffer);
}

batches *batches_create(device *vkd) {
  batches *batches = core_alloc(sizeof(struct batches));

  batches->vkd = vkd;
  utarray_alloc(batches->primitiveElements, sizeof(renderer_cache_primitive_element *));
  utarray_alloc(batches->array, sizeof(batch));

  batches->_primitiveElementsDirty = false;

  return batches;
}

void batches_destroy(batches *batches) {
  utarray_free(batches->primitiveElements);

  batches_reset(batches);
  utarray_free(batches->array);

  core_free(batches);
}

void batches_reset(batches *batches) {
  utarray_foreach_elem_it (batch *, batch, batches->array) { batch_deinit(batch); }
}

void batches_add_primitive_element(batches *batches,
                                   renderer_cache_primitive_element *primitiveElement) {
  log_debug("adding primitive element to batches");

  utarray_push_back(batches->primitiveElements, &primitiveElement);
  batches->_primitiveElementsDirty = true;
}

static int sort_primitive_elements_before_update_cmp(const void *aPtr, const void *bPtr) {
  const renderer_cache_primitive_element *a = *(const renderer_cache_primitive_element **)aPtr;
  const renderer_cache_primitive_element *b = *(const renderer_cache_primitive_element **)bPtr;

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

static void sort_primitive_elements_before_update(batches *batches) {
  log_debug("sorting primitive elements before batches update");
  // PERF: Sorting primitives should depend on selected instancing policy.
  assert(utarray_len(batches->primitiveElements) > 0);

  // sort primitive elements in order to minimize number of instances
  utarray_sort(batches->primitiveElements, sort_primitive_elements_before_update_cmp);
}

void batches_update(batches *batches, batch_instancing_policy policy) {
  if (!batches->_primitiveElementsDirty) {
    return;
  }

  // sort renderer cache elements and update attributes
  sort_primitive_elements_before_update(batches);
  assert(utarray_len(batches->primitiveElements) > 0);

  log_debug("updating batches");
  batches_reset(batches);

  renderer_cache_primitive_element *lastPrimitiveElement = NULL;
  batch *lastBatch = NULL;
  size_t instanceId = 0;

  utarray_foreach_elem_deref (renderer_cache_primitive_element *, primitiveElement,
                              batches->primitiveElements) {
    if (!primitiveElement->visible) {
      continue;
    }

    if (lastPrimitiveElement == NULL) {
      lastPrimitiveElement = primitiveElement;
    }

    if (lastBatch == NULL ||
        !batch_matching_primitive_element(lastBatch, primitiveElement, policy)) {
      batch newBatch;
      batch_init(&newBatch, primitiveElement, instanceId);
      utarray_push_back(batches->array, &newBatch);
      lastPrimitiveElement = primitiveElement;
      lastBatch = utarray_back(batches->array);
    } else {
      batch_add_primitive_element(lastBatch, primitiveElement, instanceId);
    }

    instanceId++;
  }

  batches->_primitiveElementsDirty = false;
}

void batches_record_draw_command(batches *batches, VkCommandBuffer commandBuffer,
                                 batches_buffer *batchesData) {

  size_t drawCommandCount = utarray_len(batches->array);
  verify(drawCommandCount <= batches->vkd->limits.maxDrawIndirectCommands);
  if (drawCommandCount == 0) {
    return;
  }

  VkDrawIndexedIndirectCommand drawCommands[drawCommandCount];
  size_t drawCommandIdx = 0;
  utarray_foreach_elem_it (batch *, batch, batches->array) {
    batch_update_draw_command(batch);
    assert(batch->drawCommand.firstIndex != INT32_MAX);
    assert(batch->drawCommand.vertexOffset != INT32_MAX);

    drawCommands[drawCommandIdx++] = batch->drawCommand;
  }

  batchesData->indirectDrawBufferElement.data = drawCommands;
  batchesData->indirectDrawBufferElement.size = sizeof(drawCommands);
  buffer_element_update(&batchesData->indirectDrawBufferElement);

  size_t drawCommandStride = sizeof(drawCommands[0]);
  vkCmdDrawIndexedIndirect(commandBuffer, batchesData->indirectDrawBufferElement.buffer->buffer, 0,
                           drawCommandCount, drawCommandStride);
}

void batches_debug_print(batches *batches) {
  log_debug("BATCHES:");
  log_debug("batch count: %zu", utarray_len(batches->array));
  utarray_foreach_elem_it (batch *, batch, batches->array) { batch_debug_print(batch); }
}
