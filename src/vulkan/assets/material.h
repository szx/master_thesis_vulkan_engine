/* Material state.
 * Represents PBR parameters of a primitive.
 * Part of a primitive.
 */
#pragma once

#include "common.h"
#include "texture.h"

typedef struct asset_material {
  vec4 baseColorFactor; ///< sRGBA
  float metallicFactor;
  float roughnessFactor;
  asset_texture *baseColorTexture;
  asset_texture *metallicRoughnessTexture;
  asset_texture *normalMapTexture;

  VULKAN_ASSET_FIELD_DECLS(material)
} asset_material;

VULKAN_ASSET_FUNC_DECLS(material)
