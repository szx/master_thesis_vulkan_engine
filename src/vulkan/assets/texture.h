/* Texture state.
 * Represents PBR parameters of a primitive.
 * Part of a material.
 */
#pragma once

#include "common.h"

/* image */

typedef struct asset_image {
  /// Dimensions of one face.
  uint32_t width, height, depth, channels;
  /// Number of faces.
  uint32_t faceCount;
  /// Type of image.
  image_type type;
  /// uint8_t array with image data
  /// For cubemap contains consecutive image data of right, left, top, bottom, back and front faces
  /// with total size == 6*width*height*depth*channels.
  UT_array *data; // uint8_t array

  VULKAN_ASSET_FIELD_DECLS(image)
} asset_image;

VULKAN_ASSET_FUNC_DECLS(image)

/* sampler */

typedef struct asset_sampler {
  VkFilter magFilter, minFilter;
  VkSamplerAddressMode addressModeU, addressModeV;

  VULKAN_ASSET_FIELD_DECLS(sampler)
} asset_sampler;

VULKAN_ASSET_FUNC_DECLS(sampler)

/* texture */

typedef struct asset_texture {
  asset_image *image;
  asset_sampler *sampler;

  VULKAN_ASSET_FIELD_DECLS(texture)
} asset_texture;

VULKAN_ASSET_FUNC_DECLS(texture)

asset_texture *asset_texture_create_from_params(image_type type, uint32_t width, uint32_t height,
                                                uint32_t depth, uint32_t channels);

/* skybox */

typedef struct asset_skybox {
  UT_string *name;
  asset_texture *cubemapTexture;

  VULKAN_ASSET_FIELD_DECLS(skybox)
} asset_skybox;

VULKAN_ASSET_FUNC_DECLS(skybox)

/* font */

typedef struct asset_font {
  UT_string *name;
  asset_texture *fontTexture;
  UT_string *characters;
  uint32_t characterSize;

  VULKAN_ASSET_FIELD_DECLS(font)
} asset_font;

VULKAN_ASSET_FUNC_DECLS(font)
