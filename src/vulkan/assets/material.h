/* Material state.
 * Represents PBR parameters of a primitive.
 * Part of a primitive.
 */
#pragma once

#include "common.h"
#include "texture.h"

typedef struct vulkan_asset_material {
  vec4 baseColorFactor; ///< sRGBA
  float metallicFactor;
  float roughnessFactor;
  vulkan_asset_texture *baseColorTexture;
  vulkan_asset_texture *metallicRoughnessTexture;
  vulkan_asset_texture *normalMapTexture;

  VULKAN_ASSET_FIELD_DECLS(material)
} vulkan_asset_material;

VULKAN_ASSET_FUNC_DECLS(material)
