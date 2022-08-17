#include "image.h"

image *image_create(device *vkd, image_type type, uint32_t width, uint32_t height,
                    uint32_t channels, const char *debugName) {
  verify(width <= vkd->limits.maxImageDimension2D);
  verify(height <= vkd->limits.maxImageDimension2D);

  image *image = core_alloc(sizeof(struct image));

  image->vkd = vkd;
  image->type = type;
  image->width = width;
  image->height = height;
  image->channels = channels;
  image->sampleCount = VK_SAMPLE_COUNT_1_BIT;
  image->name[0] = debugName;
  if (image->type == image_type_material_base_color) {
    image->mipLevelCount = 1 + (uint32_t)floor(log2((double)MAX(image->width, image->height)));
    image->arrayLayers = 1;
    image->format = find_image_format(vkd, image->type, image->channels);
    image->tiling = VK_IMAGE_TILING_OPTIMAL;
    image->createFlags = 0;
    image->usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    image->aspectFlags = find_image_aspects(image->type);
    image->viewType = VK_IMAGE_VIEW_TYPE_2D;
    image->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->copyDataToDevice = true;
    image->name[1] = "material base color image";
  } else if (image->type == image_type_material_parameters) {
    image->mipLevelCount = 1;
    image->arrayLayers = 1;
    image->format = find_image_format(vkd, image->type, image->channels);
    image->tiling = VK_IMAGE_TILING_OPTIMAL;
    image->createFlags = 0;
    image->usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    image->aspectFlags = find_image_aspects(image->type);
    image->viewType = VK_IMAGE_VIEW_TYPE_2D;
    image->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->copyDataToDevice = true;
    image->name[1] = "material parameters image";
  } else if (image->type == image_type_material_normal_map) {
    image->mipLevelCount = 1;
    image->arrayLayers = 1;
    image->format = find_image_format(vkd, image->type, image->channels);
    image->tiling = VK_IMAGE_TILING_OPTIMAL;
    image->createFlags = 0;
    image->usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    image->aspectFlags = find_image_aspects(image->type);
    image->viewType = VK_IMAGE_VIEW_TYPE_2D;
    image->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->copyDataToDevice = true;
    image->name[1] = "material normal map image";
  } else if (image->type == image_type_cubemap) {
    image->mipLevelCount = 1;
    image->arrayLayers = 6;
    image->format = find_image_format(vkd, image->type, image->channels);
    image->tiling = VK_IMAGE_TILING_OPTIMAL;
    image->createFlags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    image->usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    image->aspectFlags = find_image_aspects(image->type);
    image->viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    image->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->copyDataToDevice = true;
    image->name[1] = "cubemap image";
  } else if (image->type == image_type_font_bitmap) {
    image->mipLevelCount = 1;
    image->arrayLayers = 1;
    image->format = find_image_format(vkd, image->type, image->channels);
    image->tiling = VK_IMAGE_TILING_OPTIMAL;
    image->createFlags = 0;
    image->usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    image->aspectFlags = find_image_aspects(image->type);
    image->viewType = VK_IMAGE_VIEW_TYPE_2D;
    image->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->copyDataToDevice = true;
    image->name[1] = "font bitmap image";
  } else if (image->type == image_type_offscreen_f16) {
    image->mipLevelCount = 1;
    image->arrayLayers = 1;
    image->format = find_image_format(vkd, image->type, image->channels);
    image->tiling = VK_IMAGE_TILING_OPTIMAL;
    image->createFlags = 0;
    image->usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    image->aspectFlags = find_image_aspects(image->type);
    image->viewType = VK_IMAGE_VIEW_TYPE_2D;
    image->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->copyDataToDevice = false;
    image->name[1] = "offscreen image floating-point 16 bit";
  } else if (image->type == image_type_offscreen_depth_buffer) {
    image->mipLevelCount = 1;
    image->arrayLayers = 1;
    image->format = find_image_format(vkd, image->type, image->channels);
    image->tiling = VK_IMAGE_TILING_OPTIMAL;
    image->createFlags = 0;
    image->usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image->aspectFlags = find_image_aspects(image->type);
    image->viewType = VK_IMAGE_VIEW_TYPE_2D;
    image->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->copyDataToDevice = false;
    image->name[1] = "offscreen depth buffer image";
  } else if (image->type == image_type_offscreen_r8) {
    image->mipLevelCount = 1;
    image->arrayLayers = 1;
    image->format = find_image_format(vkd, image->type, image->channels);
    image->tiling = VK_IMAGE_TILING_OPTIMAL;
    image->createFlags = 0;
    image->usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    image->aspectFlags = find_image_aspects(image->type);
    image->viewType = VK_IMAGE_VIEW_TYPE_2D;
    image->memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image->copyDataToDevice = false;
    image->name[1] = "offscreen image R8 8 bit";
  } else {
    UNREACHABLE;
  }

  image->image = VK_NULL_HANDLE;
  image->imageMemory = VK_NULL_HANDLE;
  image->imageView = VK_NULL_HANDLE;

  image->resident = false;
  image->dirty = true;
  image->texture = NULL;

  image_make_resident(image);

  assert(image->image != VK_NULL_HANDLE);
  assert(image->imageMemory != VK_NULL_HANDLE);
  assert(image->imageView != VK_NULL_HANDLE);

  return image;
}

void image_destroy(image *image) {
  if (image->resident) {
    vkDestroyImageView(image->vkd->device, image->imageView, vka);
    vkDestroyImage(image->vkd->device, image->image, vka);
    vkFreeMemory(image->vkd->device, image->imageMemory, vka);
    image->resident = false;
  }
  core_free(image);
}

void image_make_resident(image *image) {
  if (!image->resident) {
    verify(image->width > 0 && image->height > 0);
    if (image->image != VK_NULL_HANDLE) {
      vkDestroyImage(image->vkd->device, image->image, vka);
    }
    image->image = device_create_image(image->vkd, image->width, image->height,
                                       image->mipLevelCount, image->arrayLayers, image->sampleCount,
                                       image->format, image->tiling, image->createFlags,
                                       image->usageFlags, "%s %s", image->name[0], image->name[1]);

    if (image->imageMemory != VK_NULL_HANDLE) {
      vkFreeMemory(image->vkd->device, image->imageMemory, vka);
    }
    image->imageMemory =
        device_create_image_memory(image->vkd, image->image, image->memoryPropertyFlags, "%s %s",
                                   image->name[0], image->name[1]);

    if (image->imageView != VK_NULL_HANDLE) {
      vkDestroyImageView(image->vkd->device, image->imageView, vka);
    }
    image->imageView = device_create_image_view(
        image->vkd, image->image, image->viewType, image->format, image->aspectFlags,
        image->mipLevelCount, image->arrayLayers, "%s %s", image->name[0], image->name[1]);
    image->resident = true;
  }
}

void image_update(image *image, asset_texture *texture) {
  image->texture = texture;
  image->dirty = true;
}

void image_send_to_device(image *image) {
  image_make_resident(image);

  if (!image->dirty) {
    return;
  }
  if (image->texture != NULL && image->copyDataToDevice) {
    size_t pixelSize = image->texture->image->channels;
    size_t texelSize = device_format_size(image->format);
    assert(pixelSize <= texelSize);
    size_t emptyComponents = texelSize - pixelSize;
    size_t texelNum = image->width * image->height;
    size_t totalLayerPixelSize = texelNum * pixelSize;
    size_t totalLayerTexelSize = texelNum * texelSize;
    size_t totalTexelSize = image->arrayLayers * texelNum * texelSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    device_create_staging_buffer(image->vkd, totalTexelSize, &stagingBuffer, &stagingBufferMemory);

    void *data;
    vkMapMemory(image->vkd->device, stagingBufferMemory, 0, totalTexelSize, 0, &data);
    uint8_t *bytes = data;
    for (size_t layerIdx = 0; layerIdx < image->arrayLayers; layerIdx++) {
      size_t pixelLayerOffset = layerIdx * totalLayerPixelSize;
      size_t texelLayerOffset = layerIdx * totalLayerTexelSize;
      for (size_t i = 0; i < texelNum; i++) {
        core_memcpy(bytes + texelLayerOffset + i * texelSize,
                    utarray_eltptr(image->texture->image->data, pixelLayerOffset + i * pixelSize),
                    pixelSize);
        if (emptyComponents > 0) {
          core_memset(bytes + i * texelSize + pixelSize, 0, emptyComponents);
        }
      }
    }
    vkUnmapMemory(image->vkd->device, stagingBufferMemory);

    device_one_shot_transition_image_layout(image->vkd, image->image, image->aspectFlags,
                                            VK_IMAGE_LAYOUT_UNDEFINED,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    device_one_shot_copy_buffer_to_image(image->vkd, stagingBuffer, image->image, image->width,
                                         image->height, image->arrayLayers);

    vkDestroyBuffer(image->vkd->device, stagingBuffer, vka);
    vkFreeMemory(image->vkd->device, stagingBufferMemory, vka);

    device_one_shot_generate_mipmaps(image->vkd, image->image, image->format, image->width,
                                     image->height, image->mipLevelCount);

    image->texture = NULL;
  }
  image->dirty = false;
}

void image_debug_print(image *image, int indent) {
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

VkFormat find_image_format(device *vkd, image_type imageType, uint32_t channels) {
  if (imageType == image_type_swap_chain) {
    assert(vkd->swapChainInfo.imageFormat != VK_FORMAT_UNDEFINED);
    return vkd->swapChainInfo.imageFormat;
  }

  image_type_info info = vulkanImageTypeInfo[imageType];

  if (info.forcedChannels > 0) {
    channels = info.forcedChannels;
  }
  assert(channels > 0 && channels <= 4);

  if (info.depthFormat) {
    assert(channels == 1);
    VkFormat formats[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                          VK_FORMAT_D24_UNORM_S8_UINT};
    return device_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, formats,
                                        array_size(formats));
  }

  if (info.colorFormat) {
#define GET_FORMAT(_var, _bits, _format)                                                           \
  if (info.componentBits == _bits) {                                                               \
    _var = VK_FORMAT_##_format;                                                                    \
  }
    VkFormat r = VK_FORMAT_UNDEFINED;
    VkFormat rg = VK_FORMAT_UNDEFINED;
    VkFormat rgb = VK_FORMAT_UNDEFINED;
    VkFormat rgba = VK_FORMAT_UNDEFINED;
    if (info.sRGBEncoding) {
      GET_FORMAT(r, 8, R8_SRGB)
      GET_FORMAT(rg, 8, R8G8_SRGB)
      GET_FORMAT(rgb, 8, R8G8B8_SRGB)
      GET_FORMAT(rgba, 8, R8G8B8A8_SRGB)
    } else if (info.uNormEncoding) {
      GET_FORMAT(r, 8, R8_UNORM)
      GET_FORMAT(r, 16, R16_UNORM)
      GET_FORMAT(rg, 8, R8G8_UNORM)
      GET_FORMAT(rg, 16, R16G16_UNORM)
      GET_FORMAT(rgb, 8, R8G8B8_UNORM)
      GET_FORMAT(rgb, 16, R16G16B16_UNORM)
      GET_FORMAT(rgba, 8, R8G8B8A8_UNORM)
      GET_FORMAT(rgba, 16, R16G16B16A16_UNORM)
    } else if (info.floatingPointEncoding) {
      GET_FORMAT(r, 16, R16_SFLOAT)
      GET_FORMAT(r, 32, R32_SFLOAT)
      GET_FORMAT(r, 64, R64_SFLOAT)
      GET_FORMAT(rg, 16, R16G16_SFLOAT)
      GET_FORMAT(rg, 32, R32G32_SFLOAT)
      GET_FORMAT(rg, 64, R64G64_SFLOAT)
      GET_FORMAT(rgb, 16, R16G16B16_SFLOAT)
      GET_FORMAT(rgb, 32, R32G32B32_SFLOAT)
      GET_FORMAT(rgb, 64, R64G64B64_SFLOAT)
      GET_FORMAT(rgba, 16, R16G16B16A16_SFLOAT)
      GET_FORMAT(rgba, 32, R32G32B32A32_SFLOAT)
      GET_FORMAT(rgba, 64, R64G64B64A64_SFLOAT)
    }
    assert(r != VK_FORMAT_UNDEFINED);
    assert(rg != VK_FORMAT_UNDEFINED);
    assert(rgb != VK_FORMAT_UNDEFINED);
    assert(rgba != VK_FORMAT_UNDEFINED);
#undef GET_FORMAT

    if (channels == 1) {
      VkFormat formats[] = {r, rg, rgb, rgba};
      return device_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                          VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                          array_size(formats));
    }
    if (channels == 2) {
      VkFormat formats[] = {rg, rgb, rgba};
      return device_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                          VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                          array_size(formats));
    }
    if (channels == 3) {
      VkFormat formats[] = {rgb, rgba};
      return device_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                          VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                          array_size(formats));
    }
    if (channels == 4) {
      VkFormat formats[] = {rgba};
      return device_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                          VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                          array_size(formats));
    }
  }

  UNREACHABLE;
}

VkImageAspectFlags find_image_aspects(image_type imageType) {
  assert(imageType != image_type_swap_chain);

  image_type_info info = vulkanImageTypeInfo[imageType];
  if (info.depthFormat) {
    return VK_IMAGE_ASPECT_DEPTH_BIT;
  }
  return VK_IMAGE_ASPECT_COLOR_BIT;
}
