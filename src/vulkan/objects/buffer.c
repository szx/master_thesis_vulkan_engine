#include "buffer.h"

void vulkan_buffer_element_update(vulkan_buffer_element *bufferElement) {
  if (bufferElement->data == NULL) {
    log_warn("attempted to update buffer with NULL pointer");
    return;
  }

  bool useStagingBuffer =
      (bufferElement->buffer->memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0;
  assert(!useStagingBuffer);

  vulkan_buffer *buffer = bufferElement->buffer;
  void *data;
  vkMapMemory(buffer->vkd->device, buffer->bufferMemory, bufferElement->bufferOffset,
              bufferElement->size, 0, &data);
  core_memcpy(data, bufferElement->data, bufferElement->size);
  vkUnmapMemory(buffer->vkd->device, buffer->bufferMemory);
}

void vulkan_buffer_element_debug_print(vulkan_buffer_element *bufferElement, int indent) {
  log_debug(INDENT_FORMAT_STRING "buffer element of buffer '%s':", INDENT_FORMAT_ARGS(0),
            bufferElement->buffer->name);
  log_debug(INDENT_FORMAT_STRING "size=%zu", INDENT_FORMAT_ARGS(2), bufferElement->size);
  log_debug(INDENT_FORMAT_STRING "bufferOffset=%zu", INDENT_FORMAT_ARGS(2),
            bufferElement->bufferOffset);
}

vulkan_buffer *vulkan_buffer_create(vulkan_device *vkd, vulkan_buffer_type type) {
  vulkan_buffer *buffer = core_alloc(sizeof(vulkan_buffer));

  buffer->totalSize = 0;
  utarray_alloc(buffer->elements, sizeof(vulkan_buffer_element));

  buffer->vkd = vkd;
  buffer->type = type;
  buffer->bufferUsageFlags = 0;
  buffer->memoryPropertyFlags = 0;
  buffer->name = "invalid buffer";
  if (buffer->type == vulkan_buffer_type_geometry_index) {
    buffer->bufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    buffer->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    buffer->name = "index geometry buffer";
  } else if (buffer->type == vulkan_buffer_type_geometry_vertex) {
    buffer->bufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    buffer->name = "vertex geometry buffer";
  } else if (buffer->type == vulkan_buffer_type_uniform) {
    buffer->bufferUsageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buffer->memoryPropertyFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->name = "uniform buffer";
  } else if (buffer->type == vulkan_buffer_type_indirect_draw) {
    buffer->bufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                               VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                               VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    buffer->memoryPropertyFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    buffer->name = "draw indirect buffer";
  } else {
    assert(0);
  }
  buffer->buffer = VK_NULL_HANDLE;
  buffer->bufferMemory = VK_NULL_HANDLE;

  buffer->resident = false;
  buffer->dirty = true;

  return buffer;
}

void vulkan_buffer_destroy(vulkan_buffer *buffer) {
  if (buffer->resident) {
    vkDestroyBuffer(buffer->vkd->device, buffer->buffer, vka);
    vkFreeMemory(buffer->vkd->device, buffer->bufferMemory, vka);
    buffer->resident = false;
  }
  utarray_free(buffer->elements);
  core_free(buffer);
}

vulkan_buffer_element vulkan_buffer_add(vulkan_buffer *buffer, const void *data, size_t size) {
  verify(!buffer->resident);
  vulkan_buffer_element element = {
      .buffer = buffer, .data = data, .size = size, .bufferOffset = buffer->totalSize};
  buffer->totalSize += element.size;
  utarray_push_back(buffer->elements, &element);
  buffer->dirty = true;
  // FIXME: Possible alignment/granularity problems for buffer elements?
  return element;
}

void vulkan_buffer_make_resident(vulkan_buffer *buffer) {
  if (!buffer->resident) {
    verify(buffer->totalSize > 0);
    vulkan_create_buffer(buffer->vkd, buffer->totalSize, buffer->bufferUsageFlags,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &buffer->buffer,
                         &buffer->bufferMemory, buffer->name);
    buffer->resident = true;
  }
}

void vulkan_buffer_send_to_device(vulkan_buffer *buffer) {
  if (!buffer->dirty) {
    return;
  }

  vulkan_buffer_make_resident(buffer);

  bool useStagingBuffer = (buffer->memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0;
  if (useStagingBuffer) {
    // TODO: Reuse staging buffer.
    // TODO: Free geometry buffer data if geometry buffer is device local.
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    vulkan_create_buffer(buffer->vkd, buffer->totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         &stagingBuffer, &stagingBufferMemory, "staging buffer for geometry");

    void *data;
    vkMapMemory(buffer->vkd->device, stagingBufferMemory, 0, buffer->totalSize, 0, &data);
    utarray_foreach_elem_deref (vulkan_buffer_element, element, buffer->elements) {
      core_memcpy(data, element.data, element.size);
    }
    vkUnmapMemory(buffer->vkd->device, stagingBufferMemory);

    vulkan_copy_buffer_to_buffer(buffer->vkd, stagingBuffer, buffer->buffer, buffer->totalSize);

    vkDestroyBuffer(buffer->vkd->device, stagingBuffer, vka);
    vkFreeMemory(buffer->vkd->device, stagingBufferMemory, vka);
  } else {
    void *data;
    vkMapMemory(buffer->vkd->device, buffer->bufferMemory, 0, buffer->totalSize, 0, &data);
    utarray_foreach_elem_deref (vulkan_buffer_element, element, buffer->elements) {
      uint8_t *bytes = data;
      core_memcpy(bytes + element.bufferOffset, element.data, element.size);
    }
    vkUnmapMemory(buffer->vkd->device, buffer->bufferMemory);
  }
  buffer->dirty = false;
}
