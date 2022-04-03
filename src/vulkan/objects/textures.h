/* Vulkan API helper objects. */

#pragma once

#include "../data/material.h"
#include "camera.h"
#include "image.h"
#include "render_cache_list.h"
#include "shader.h"

/// Hash table for textures and their associated images and samplers.
typedef struct vulkan_textures_element {
  vulkan_data_texture *texture; ///< Pointer. Key.
  vulkan_image *image;          ///< GPU image
  VkSampler sampler;            ///< Sampler used to read images.
  uint32_t textureIdx;          ///< Index in array of textures bound by descriptor set.
  UT_hash_handle hh;
} vulkan_textures_element;

vulkan_textures_element *vulkan_textures_element_create(vulkan_data_texture *texture,
                                                        vulkan_device *vkd);
void vulkan_textures_element_destroy(vulkan_textures_element *element);

typedef struct vulkan_textures {
  /* CPU state */
  vulkan_device *vkd;                        ///< Pointer.
  vulkan_render_cache_list *renderCacheList; ///< Pointer.

  /* GPU state */
  vulkan_textures_element *elements;

} vulkan_textures;

vulkan_textures *vulkan_textures_create(vulkan_device *vkd,
                                        vulkan_render_cache_list *renderCacheList);
void vulkan_textures_destroy(vulkan_textures *textures);

void vulkan_textures_update(vulkan_textures *textures);

void vulkan_textures_send_to_device(vulkan_textures *textures);

void vulkan_textures_add_texture(vulkan_textures *textures, vulkan_data_texture *texture);

void vulkan_textures_debug_print(vulkan_textures *textures);

void glsl_add_textures(UT_string *s, uint32_t set, uint32_t binding, vulkan_textures *textures);
