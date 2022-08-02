#include "buffer.h"

void buffer_element_update(buffer_element *bufferElement) {
  if (bufferElement->data == NULL) {
    log_warn("attempted to update buffer with NULL pointer");
    return;
  }

  bool useStagingBuffer =
      (bufferElement->buffer->memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0;
  assert(!useStagingBuffer);

  buffer *buffer = bufferElement->buffer;
  void *data;
  vkMapMemory(buffer->vkd->device, buffer->bufferMemory, bufferElement->bufferOffset,
              bufferElement->size, 0, &data);
  core_memcpy(data, bufferElement->data, bufferElement->size);
  vkUnmapMemory(buffer->vkd->device, buffer->bufferMemory);
}

void buffer_element_debug_print(buffer_element *bufferElement, int indent) {
  log_debug(INDENT_FORMAT_STRING "buffer element of buffer '%s':", INDENT_FORMAT_ARGS(0),
            bufferElement->buffer->name);
  log_debug(INDENT_FORMAT_STRING "size=%zu", INDENT_FORMAT_ARGS(2), bufferElement->size);
  log_debug(INDENT_FORMAT_STRING "bufferOffset=%zu", INDENT_FORMAT_ARGS(2),
            bufferElement->bufferOffset);
}

buffer *buffer_create(device *vkd, buffer_type type) {
  buffer *buffer = core_alloc(sizeof(struct buffer));

  buffer->totalSize = 0;
  utarray_alloc(buffer->elements, sizeof(buffer_element));

  buffer->vkd = vkd;
  buffer->type = type;
  buffer->bufferUsageFlags = 0;
  buffer->memoryPropertyFlags = 0;
  buffer->name = "invalid buffer";
  if (buffer->type == buffer_type_geometry_index) {
    buffer->bufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    buffer->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    buffer->name = "index geometry buffer";
  } else if (buffer->type == buffer_type_geometry_vertex) {
    buffer->bufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    buffer->name = "vertex geometry buffer";
  } else if (buffer->type == buffer_type_uniform) {
    buffer->bufferUsageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buffer->memoryPropertyFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->name = "uniform buffer";
  } else if (buffer->type == buffer_type_indirect_draw) {
    buffer->bufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                               VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                               VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    buffer->memoryPropertyFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->name = "draw indirect buffer";
  } else {
    UNREACHABLE;
  }
  buffer->buffer = VK_NULL_HANDLE;
  buffer->bufferMemory = VK_NULL_HANDLE;

  buffer->resident = false;
  buffer->dirty = true;

  return buffer;
}

void buffer_destroy(buffer *buffer) {
  if (buffer->resident) {
    vkDestroyBuffer(buffer->vkd->device, buffer->buffer, vka);
    vkFreeMemory(buffer->vkd->device, buffer->bufferMemory, vka);
    buffer->resident = false;
  }
  utarray_free(buffer->elements);
  core_free(buffer);
}

buffer_element buffer_add(buffer *buffer, const void *data, size_t size) {
  verify(!buffer->resident);
  buffer_element element = {
      .buffer = buffer, .data = data, .size = size, .bufferOffset = buffer->totalSize};
  buffer->totalSize += element.size;
  if (buffer->type == buffer_type_uniform) {
    // Round buffer size to minimum alignment required by vkUpdateDescriptorSets().
    // It makes so next buffer offset has correct alignment.
    buffer->totalSize += buffer->vkd->limits.minUniformBufferOffsetAlignment -
                         (buffer->totalSize % buffer->vkd->limits.minUniformBufferOffsetAlignment);
  }
  utarray_push_back(buffer->elements, &element);
  buffer->dirty = true;
  // FIXME: More possible alignment/granularity problems for buffer elements?
  return element;
}

void buffer_make_resident(buffer *buffer) {
  if (!buffer->resident) {
    verify(buffer->totalSize > 0);
    create_buffer(buffer->vkd, buffer->totalSize, buffer->bufferUsageFlags,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &buffer->buffer, &buffer->bufferMemory,
                  buffer->name);
    buffer->resident = true;
  }
}

void buffer_send_to_device(buffer *buffer) {
  if (!buffer->dirty) {
    return;
  }

  buffer_make_resident(buffer);

  bool useStagingBuffer = (buffer->memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0;
  if (useStagingBuffer) {
    // TODO: Reuse staging buffer.
    // TODO: Free geometry buffer data if geometry buffer is device local.
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_buffer(buffer->vkd, buffer->totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  &stagingBuffer, &stagingBufferMemory, "staging buffer for geometry");

    void *data;
    vkMapMemory(buffer->vkd->device, stagingBufferMemory, 0, buffer->totalSize, 0, &data);
    utarray_foreach_elem_deref (buffer_element, element, buffer->elements) {
      core_memcpy(data, element.data, element.size);
    }
    vkUnmapMemory(buffer->vkd->device, stagingBufferMemory);

    copy_buffer_to_buffer(buffer->vkd, stagingBuffer, buffer->buffer, buffer->totalSize);

    vkDestroyBuffer(buffer->vkd->device, stagingBuffer, vka);
    vkFreeMemory(buffer->vkd->device, stagingBufferMemory, vka);
  } else {
    void *data;
    vkMapMemory(buffer->vkd->device, buffer->bufferMemory, 0, buffer->totalSize, 0, &data);
    utarray_foreach_elem_deref (buffer_element, element, buffer->elements) {
      uint8_t *bytes = data;
      core_memcpy(bytes + element.bufferOffset, element.data, element.size);
    }
    vkUnmapMemory(buffer->vkd->device, buffer->bufferMemory);
  }
  buffer->dirty = false;
}
