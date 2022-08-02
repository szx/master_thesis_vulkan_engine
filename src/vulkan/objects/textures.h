/* Vulkan API helper objects. */

#pragma once

#include "../assets/material.h"
#include "image.h"

/// Hash table for textures and their associated images and samplers.
typedef struct textures_texture_element {
  asset_texture *texture; ///< Pointer. Key.
  image *image;           ///< GPU image
  VkSampler sampler;      ///< Sampler used to read images.
  uint32_t textureIdx;    ///< Index in array of textures bound by descriptor set.
  UT_hash_handle hh;
} textures_texture_element;

textures_texture_element *textures_texture_element_create(asset_texture *texture, device *vkd,
                                                          const char *debugName);
void textures_texture_element_destroy(textures_texture_element *element);

/// Hash table for materials and their associated textures.
typedef struct textures_material_element {
  asset_material *material;                                  ///< Pointer. Key.
  textures_texture_element *baseColorTextureElement;         ///< Pointer.
  textures_texture_element *metallicRoughnessTextureElement; ///< Pointer.
  textures_texture_element *normalMapTextureElement;         ///< Pointer.
  uint32_t materialIdx; ///< Index in array of materials bound by descriptor set.
  UT_hash_handle hh;
} textures_material_element;

textures_material_element *
textures_material_element_create(asset_material *material,
                                 textures_texture_element *baseColorTextureElement,
                                 textures_texture_element *metallicRoughnessTextureElement,
                                 textures_texture_element *normalMapTextureElement);

void textures_material_element_destroy(textures_material_element *element);

typedef struct textures {
  /* CPU state */
  device *vkd; ///< Pointer.

  /* GPU state */
  textures_texture_element *textureElements;
  textures_material_element *materialElements;

} textures;

textures *textures_create(device *vkd);
void textures_destroy(textures *textures);

void textures_update(textures *textures);

void textures_send_to_device(textures *textures);

textures_material_element *textures_add_material(textures *textures, asset_material *material);

textures_texture_element *textures_add_texture(textures *textures, asset_texture *texture,
                                               const char *debugName);

void textures_debug_print(textures *textures, int indent);

void glsl_add_textures(UT_string *s, uint32_t set, uint32_t binding, textures *textures);
