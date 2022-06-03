/* Vulkan API helper objects. */
#pragma once

#include "../assets/texture.h"
#include "../common.h"

/// Image.
/// Used to store data in memory.
typedef struct vulkan_image {
  vulkan_device *vkd; ///< Pointer.

  vulkan_image_type type;
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

  bool resident;                 ///< True if image created in GPU memory.
  bool dirty;                    ///< True if CPU to GPU transfer required.
  vulkan_asset_texture *texture; ///< Pointer to texture used to update image.
} vulkan_image;

vulkan_image *vulkan_image_create(vulkan_device *vkd, vulkan_image_type type, uint32_t width,
                                  uint32_t height, uint32_t channels, const char *debugName);
void vulkan_image_destroy(vulkan_image *image);

void vulkan_image_make_resident(vulkan_image *image);

void vulkan_image_update(vulkan_image *image, vulkan_asset_texture *texture);

void vulkan_image_send_to_device(vulkan_image *image);

void vulkan_image_debug_print(vulkan_image *image, int indent);

VkFormat vulkan_find_image_format(vulkan_device *vkd, vulkan_image_type imageType,
                                  uint32_t channels);
