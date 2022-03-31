/* Vulkan API helper objects. */
#pragma once

#include "../common.h"

typedef enum vulkan_image_type {
  vulkan_image_type_depth_buffer,
  vulkan_image_type_material_r8g8b8a8,
  vulkan_image_type_count
} vulkan_image_type;

/// Image.
/// Used to store data in memory.
typedef struct vulkan_image {
  vulkan_device *vkd; ///< Pointer.

  vulkan_image_type type;
  uint32_t width;
  uint32_t height;
  uint32_t mipLevelCount;
  uint32_t arrayLayers;
  VkSampleCountFlagBits sampleCount;
  VkFormat format;
  VkImageTiling tiling;
  VkImageCreateFlags createFlags;
  VkImageUsageFlags usageFlags;
  VkMemoryPropertyFlags memoryPropertyFlags;
  VkImageAspectFlags aspectFlags;
  const char *name;

  VkImage image;
  VkDeviceMemory imageMemory;
  VkImageView imageView;

  bool resident; ///< True if image created in GPU memory.
  bool dirty;    ///< True if CPU to GPU transfer required.
} vulkan_image;

vulkan_image *vulkan_image_create(vulkan_device *vkd, vulkan_image_type type, uint32_t width,
                                  uint32_t height);
void vulkan_image_destroy(vulkan_image *image);

void vulkan_image_make_resident(vulkan_image *image);
void vulkan_image_send_to_device(vulkan_image *image);

void vulkan_image_debug_print(vulkan_image *image, int indent);
