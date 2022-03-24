#include "image.h"

vulkan_image *vulkan_image_create(vulkan_device *vkd, vulkan_image_type type, uint32_t width,
                                  uint32_t height) {
  vulkan_image *image = core_alloc(sizeof(vulkan_image));

  image->vkd = vkd;
  image->type = type;
  image->width = width;
  image->height = height;
  image->arrayLayers = 1;
  image->sampleCount = VK_SAMPLE_COUNT_1_BIT;
  if (image->type == vulkan_image_type_depth_buffer) {
    image->mipLevelCount = 1;
    image->format = vulkan_find_depth_format(vkd);
    image->tiling = VK_IMAGE_TILING_OPTIMAL;
    image->createFlags = 0;
    image->usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image->aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
    image->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->name = "depth buffer image";
  } else {
    assert(0);
  }

  image->image = VK_NULL_HANDLE;
  image->imageMemory = VK_NULL_HANDLE;
  image->imageView = VK_NULL_HANDLE;

  image->resident = false;
  image->dirty = true;

  vulkan_image_make_resident(image);

  assert(image->image != VK_NULL_HANDLE);
  assert(image->imageMemory != VK_NULL_HANDLE);
  assert(image->imageView != VK_NULL_HANDLE);

  return image;
}

void vulkan_image_destroy(vulkan_image *image) {
  if (image->resident) {
    vkDestroyImageView(image->vkd->device, image->imageView, vka);
    vkDestroyImage(image->vkd->device, image->image, vka);
    vkFreeMemory(image->vkd->device, image->imageMemory, vka);
    image->resident = false;
  }
  core_free(image);
}

void vulkan_image_make_resident(vulkan_image *image) {
  if (!image->resident) {
    verify(image->width > 0 && image->height > 0);
    image->image =
        vulkan_create_image(image->vkd, image->width, image->height, image->mipLevelCount,
                            image->arrayLayers, image->sampleCount, image->format, image->tiling,
                            image->createFlags, image->usageFlags, image->name);
    image->imageMemory = vulkan_create_image_memory(image->vkd, image->image,
                                                    image->memoryPropertyFlags, image->name);
    image->imageView = vulkan_create_image_view(
        image->vkd, image->image, VK_IMAGE_VIEW_TYPE_2D, image->format, image->aspectFlags,
        image->mipLevelCount, image->arrayLayers, image->name);
    image->resident = true;
  }
}

void vulkan_image_send_to_device(vulkan_image *image) {
  // NOTE: Currently only creates image.
  if (!image->dirty) {
    return;
  }

  vulkan_image_make_resident(image);
  image->dirty = false;
}

void vulkan_image_debug_print(vulkan_image *image, int indent) {
  log_debug(INDENT_FORMAT_STRING "image '%s':", INDENT_FORMAT_ARGS(0), image->name);
  log_debug(INDENT_FORMAT_STRING "width=%zu", INDENT_FORMAT_ARGS(2), image->width);
  log_debug(INDENT_FORMAT_STRING "height=%zu", INDENT_FORMAT_ARGS(2), image->height);
  log_debug(INDENT_FORMAT_STRING "mipLevelCount=%zu", INDENT_FORMAT_ARGS(2), image->mipLevelCount);
  log_debug(INDENT_FORMAT_STRING "arrayLayers=%zu", INDENT_FORMAT_ARGS(2), image->arrayLayers);
  VkSampleCountFlagBits_debug_print(image->sampleCount, indent + 2);
  log_debug(INDENT_FORMAT_STRING "format=%s", INDENT_FORMAT_ARGS(2),
            VkFormat_debug_str(image->format));
  log_debug(INDENT_FORMAT_STRING "tiling=%s", INDENT_FORMAT_ARGS(2),
            VkImageTiling_debug_str(image->tiling));
  VkImageCreateFlagBits_debug_print(image->createFlags, indent + 2);
  VkImageUsageFlagBits_debug_print(image->usageFlags, indent + 2);
  VkMemoryPropertyFlagBits_debug_print(image->memoryPropertyFlags, indent + 2);
  VkImageAspectFlagBits_debug_print(image->aspectFlags, indent + 2);
}