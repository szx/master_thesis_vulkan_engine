/* Vulkan API helper objects. */

#pragma once

#include "../assets/material.h"
#include "image.h"

VkFormat vulkan_find_texture_format(vulkan_device *vkd, vulkan_asset_texture *texture);

/// Hash table for textures and their associated images and samplers.
typedef struct vulkan_textures_texture_element {
  vulkan_asset_texture *texture; ///< Pointer. Key.
  vulkan_image *image;           ///< GPU image
  VkSampler sampler;             ///< Sampler used to read images.
  uint32_t textureIdx;           ///< Index in array of textures bound by descriptor set.
  UT_hash_handle hh;
} vulkan_textures_texture_element;

vulkan_textures_texture_element *
vulkan_textures_texture_element_create(vulkan_asset_texture *texture, vulkan_device *vkd);
void vulkan_textures_texture_element_destroy(vulkan_textures_texture_element *element);

/// Hash table for materials and their associated textures.
typedef struct vulkan_textures_material_element {
  vulkan_asset_material *material;                                  ///< Pointer. Key.
  vulkan_textures_texture_element *baseColorTextureElement;         ///< Pointer.
  vulkan_textures_texture_element *metallicRoughnessTextureElement; ///< Pointer.
  uint32_t materialIdx; ///< Index in array of materials bound by descriptor set.
  UT_hash_handle hh;
} vulkan_textures_material_element;

vulkan_textures_material_element *vulkan_textures_material_element_create(
    vulkan_asset_material *material, vulkan_textures_texture_element *baseColorTextureElement,
    vulkan_textures_texture_element *metallicRoughnessTextureElement);

void vulkan_textures_material_element_destroy(vulkan_textures_material_element *element);

typedef struct vulkan_textures {
  /* CPU state */
  vulkan_device *vkd;                        ///< Pointer.

  /* GPU state */
  vulkan_textures_texture_element *textureElements;
  vulkan_textures_material_element *materialElements;

} vulkan_textures;

vulkan_textures *vulkan_textures_create(vulkan_device *vkd);
void vulkan_textures_destroy(vulkan_textures *textures);

void vulkan_textures_update(vulkan_textures *textures);

void vulkan_textures_send_to_device(vulkan_textures *textures);

vulkan_textures_material_element *vulkan_textures_add_material(vulkan_textures *textures,
                                                               vulkan_asset_material *material);

vulkan_textures_texture_element *vulkan_textures_add_texture(vulkan_textures *textures,
                                                             vulkan_asset_texture *texture);

void vulkan_textures_debug_print(vulkan_textures *textures, int indent);

void glsl_add_textures(UT_string *s, uint32_t set, uint32_t binding, vulkan_textures *textures);
