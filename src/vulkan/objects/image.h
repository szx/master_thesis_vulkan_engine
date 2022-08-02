/* Vulkan API helper objects. */
#pragma once

#include "../assets/texture.h"
#include "../common.h"

/// Image.
/// Used to store data in memory.
typedef struct image {
  device *vkd; ///< Pointer.

  image_type type;
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  uint32_t mipLevelCount;
  uint32_t arrayLayers;
  VkSampleCountFlagBits sampleCount;
  VkFormat format;
  VkImageTiling tiling;
  VkImageCreateFlags createFlags;
  VkImageUsageFlags usageFlags;
  VkMemoryPropertyFlags memoryPropertyFlags;
  VkImageAspectFlags aspectFlags;
  VkImageViewType viewType;
  bool copyDataToDevice;
  const char *name[2];

  VkImage image;
  VkDeviceMemory imageMemory;
  VkImageView imageView;

  bool resident;          ///< True if image created in GPU memory.
  bool dirty;             ///< True if CPU to GPU transfer required.
  asset_texture *texture; ///< Pointer to texture used to update image.
} image;

image *image_create(device *vkd, image_type type, uint32_t width, uint32_t height,
                    uint32_t channels, const char *debugName);
void image_destroy(image *image);

void image_make_resident(image *image);

void image_update(image *image, asset_texture *texture);

void image_send_to_device(image *image);

void image_debug_print(image *image, int indent);

VkFormat find_image_format(device *vkd, image_type imageType, uint32_t channels);

VkImageAspectFlags find_image_aspects(image_type imageType);
