#include "textures.h"

VkFormat vulkan_find_texture_format(vulkan_device *vkd, vulkan_asset_texture *texture) {
  size_t channels = texture->image->channels;
  assert(channels > 0 && channels <= 4);
  bool sRGB = texture->image->type == vulkan_image_type_material_base_color;
  // HIRO Refactor move preferred format to vulkan_image_type decl.
  bool useHighPrecision = texture->image->type >= vulkan_image_type_g_buffer_0 &&
                          texture->image->type <= vulkan_image_type_g_buffer_count;
  bool useFloat = texture->image->type >= vulkan_image_type_g_buffer_0 &&
                  texture->image->type <= vulkan_image_type_g_buffer_count;

  VkFormat r, rg, rgb, rgba;
  if (!useHighPrecision) {
    r = sRGB ? VK_FORMAT_R8_SRGB : VK_FORMAT_R8_UNORM;
    rg = sRGB ? VK_FORMAT_R8G8_SRGB : VK_FORMAT_R8G8_UNORM;
    rgb = sRGB ? VK_FORMAT_R8G8B8_SRGB : VK_FORMAT_R8G8B8_UNORM;
    rgba = sRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
  } else {
    r = useFloat ? VK_FORMAT_R16_SFLOAT : VK_FORMAT_R16_UNORM;
    rg = useFloat ? VK_FORMAT_R16G16_SFLOAT : VK_FORMAT_R16G16_UNORM;
    rgb = useFloat ? VK_FORMAT_R16G16B16_SFLOAT : VK_FORMAT_R16G16B16_UNORM;
    rgba = useFloat ? VK_FORMAT_R16G16B16A16_SFLOAT : VK_FORMAT_R16G16B16A16_UNORM;
  }

  if (channels == 1) {
    VkFormat formats[] = {r, rg, rgb, rgba};
    return vulkan_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                        array_size(formats));
  }
  if (channels == 2) {
    VkFormat formats[] = {rg, rgb, rgba};
    return vulkan_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                        array_size(formats));
  }
  if (channels == 3) {
    VkFormat formats[] = {rgb, rgba};
    return vulkan_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                        array_size(formats));
  }
  if (channels == 4) {
    VkFormat formats[] = {rgba};
    return vulkan_find_supported_format(vkd, VK_IMAGE_TILING_OPTIMAL,
                                        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT, formats,
                                        array_size(formats));
  }
  UNREACHABLE;
}

vulkan_textures_texture_element *
vulkan_textures_texture_element_create(vulkan_asset_texture *texture, vulkan_device *vkd) {
  vulkan_textures_texture_element *element = core_alloc(sizeof(vulkan_textures_texture_element));

  element->texture = texture;
  vulkan_image_type type = element->texture->image->type;

  element->image = vulkan_image_create(vkd, type, element->texture->image->width,
                                       element->texture->image->height,
                                       vulkan_find_texture_format(vkd, texture));
  vulkan_image_update(element->image, element->texture);

  element->sampler = vulkan_create_sampler(vkd, element->image->mipLevelCount, "texture");
  static uint32_t textureIdx = 0;
  element->textureIdx = textureIdx++;

  return element;
}

void vulkan_textures_texture_element_destroy(vulkan_textures_texture_element *element) {
  vkDestroySampler(element->image->vkd->device, element->sampler, vka);
  vulkan_image_destroy(element->image);
  core_free(element);
}

vulkan_textures_material_element *vulkan_textures_material_element_create(
    vulkan_asset_material *material, vulkan_textures_texture_element *baseColorTextureElement,
    vulkan_textures_texture_element *metallicRoughnessTextureElement) {
  vulkan_textures_material_element *element = core_alloc(sizeof(vulkan_textures_material_element));

  element->material = material;
  element->baseColorTextureElement = baseColorTextureElement;
  element->metallicRoughnessTextureElement = metallicRoughnessTextureElement;

  static uint32_t materialIdx = 0;
  element->materialIdx = materialIdx++ % MAX_MATERIAL_COUNT; // FIXME: Can overwrite materials.

  return element;
}

void vulkan_textures_material_element_destroy(vulkan_textures_material_element *element) {
  core_free(element);
}

vulkan_textures *vulkan_textures_create(vulkan_device *vkd) {
  vulkan_textures *textures = core_alloc(sizeof(vulkan_textures));

  textures->vkd = vkd;
  textures->textureElements = NULL;
  textures->materialElements = NULL;

  return textures;
}

void vulkan_textures_destroy(vulkan_textures *textures) {
  uthash_foreach_elem_it(vulkan_textures_texture_element *, element, textures->textureElements) {
    HASH_DEL(textures->textureElements, element);
    vulkan_textures_texture_element_destroy(element);
  }
  uthash_foreach_elem_it(vulkan_textures_material_element *, element, textures->materialElements) {
    HASH_DEL(textures->materialElements, element);
    vulkan_textures_material_element_destroy(element);
  }

  core_free(textures);
}

void vulkan_textures_update(vulkan_textures *textures) {
  // No-op.
}

void vulkan_textures_send_to_device(vulkan_textures *textures) {
  uthash_foreach_elem_it(vulkan_textures_texture_element *, element, textures->textureElements) {
    vulkan_image_send_to_device(element->image);
  }
}

vulkan_textures_material_element *vulkan_textures_add_material(vulkan_textures *textures,
                                                               vulkan_asset_material *material) {
  vulkan_textures_material_element *element;
  HASH_FIND(hh, textures->materialElements, &material, sizeof(material), element);
  if (element != NULL) {
    return element;
  }

  log_info("adding material");
  vulkan_asset_material_debug_print(material, 0);
  element = vulkan_textures_material_element_create(
      material, vulkan_textures_add_texture(textures, material->baseColorTexture),
      vulkan_textures_add_texture(textures, material->metallicRoughnessTexture));
  HASH_ADD_PTR(textures->materialElements, material, element);
  return element;
}

vulkan_textures_texture_element *vulkan_textures_add_texture(vulkan_textures *textures,
                                                             vulkan_asset_texture *texture) {
  vulkan_textures_texture_element *element;
  HASH_FIND(hh, textures->textureElements, &texture, sizeof(texture), element);
  if (element != NULL) {
    return element;
  }

  log_info("adding texture");
  vulkan_asset_texture_debug_print(texture, 0);
  element = vulkan_textures_texture_element_create(texture, textures->vkd);
  HASH_ADD_PTR(textures->textureElements, texture, element);
  return element;
}

void vulkan_textures_debug_print(vulkan_textures *textures, int indent) {
  log_debug(INDENT_FORMAT_STRING "textures:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "texture count=%zu\n", INDENT_FORMAT_ARGS(2),
            HASH_CNT(hh, textures->textureElements));
  log_debug(INDENT_FORMAT_STRING "material count=%zu\n", INDENT_FORMAT_ARGS(2),
            HASH_CNT(hh, textures->materialElements));
}

void glsl_add_textures(UT_string *s, uint32_t set, uint32_t binding, vulkan_textures *textures) {
  utstring_printf(s, "// Declare multiple shader variables with same set and binding numbers and "
                     "different shader types.\n"
                     "// Vulkan spec allows this aliasing ( \"DescriptorSet and Binding "
                     "Assignment\", https://github.com/KhronosGroup/Vulkan-Docs/issues/1420 ).\n");
  utstring_printf(s, "layout(set = %u, binding = %u) uniform sampler2D textures2D[];\n", set,
                  binding);
  utstring_printf(s, "layout(set = %u, binding = %u) uniform samplerCube texturesCube[];\n", set,
                  binding);
}
