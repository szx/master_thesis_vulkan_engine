/* Texture state.
 * Represents PBR parameters of a primitive.
 * Part of a material.
 */
#pragma once

#include "common.h"

/* image */

typedef struct vulkan_asset_image {
  /// Dimensions of one face.
  uint32_t width, height, depth, channels;
  /// Number of faces.
  uint32_t faceCount;
  /// Type of image.
  vulkan_image_type type;
  /// uint8_t array with image data
  /// For cubemap contains consecutive image data of right, left, top, bottom, back and front faces
  /// with total size == 6*width*height*depth*channels.
  UT_array *data; // uint8_t array

  VULKAN_ASSET_FIELD_DECLS(image)
} vulkan_asset_image;

VULKAN_ASSET_FUNC_DECLS(image)

/* sampler */

typedef struct vulkan_asset_sampler {
  VkFilter magFilter, minFilter;
  VkSamplerAddressMode addressModeU, addressModeV;

  VULKAN_ASSET_FIELD_DECLS(sampler)
} vulkan_asset_sampler;

VULKAN_ASSET_FUNC_DECLS(sampler)

/* texture */

typedef struct vulkan_asset_texture {
  vulkan_asset_image *image;
  vulkan_asset_sampler *sampler;

  VULKAN_ASSET_FIELD_DECLS(texture)
} vulkan_asset_texture;

VULKAN_ASSET_FUNC_DECLS(texture)

vulkan_asset_texture *vulkan_asset_texture_create_from_params(vulkan_scene_data *sceneData,
                                                              vulkan_image_type type,
                                                              uint32_t width, uint32_t height,
                                                              uint32_t depth, uint32_t channels);

/* skybox */

typedef struct vulkan_asset_skybox {
  UT_string *name;
  vulkan_asset_texture *cubemapTexture;

  VULKAN_ASSET_FIELD_DECLS(skybox)
} vulkan_asset_skybox;

VULKAN_ASSET_FUNC_DECLS(skybox)

/* font */

typedef struct vulkan_asset_font {
  UT_string *name;
  vulkan_asset_texture *fontTexture;
  UT_string *characters;
  uint32_t characterSize;

  VULKAN_ASSET_FIELD_DECLS(font)
} vulkan_asset_font;

VULKAN_ASSET_FUNC_DECLS(font)
