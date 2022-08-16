#include "textures.h"

VkFormat get_format_by_component_bits(char componentBits, VkFormat format8, VkFormat format16,
                                      VkFormat format32) {
  if (componentBits == 8) {
    assert(format8 != VK_FORMAT_UNDEFINED);
    return format8;
  }
  if (componentBits == 16) {
    assert(format16 != VK_FORMAT_UNDEFINED);
    return format16;
  }
  if (componentBits == 32) {
    assert(format32 != VK_FORMAT_UNDEFINED);
    return format32;
  }
  UNREACHABLE;
}

textures_texture_element *textures_texture_element_create(asset_texture *texture, device *vkd,
                                                          const char *debugName) {
  textures_texture_element *element = core_alloc(sizeof(textures_texture_element));

  element->texture = texture;
  image_type type = element->texture->image->type;

  element->image =
      image_create(vkd, type, element->texture->image->width, element->texture->image->height,
                   element->texture->image->channels, debugName);
  image_update(element->image, element->texture);

  element->sampler = device_create_sampler(vkd, element->image->mipLevelCount, "texture");
  static uint32_t textureIdx = 0;
  element->textureIdx = textureIdx++;

  return element;
}

void textures_texture_element_destroy(textures_texture_element *element) {
  vkDestroySampler(element->image->vkd->device, element->sampler, vka);
  image_destroy(element->image);
  core_free(element);
}

textures_material_element *
textures_material_element_create(asset_material *material,
                                 textures_texture_element *baseColorTextureElement,
                                 textures_texture_element *metallicRoughnessTextureElement,
                                 textures_texture_element *normalMapTextureElement) {
  textures_material_element *element = core_alloc(sizeof(textures_material_element));

  element->material = material;
  element->baseColorTextureElement = baseColorTextureElement;
  element->metallicRoughnessTextureElement = metallicRoughnessTextureElement;
  element->normalMapTextureElement = normalMapTextureElement;

  static uint32_t materialIdx = 0;
  element->materialIdx = materialIdx++ % MAX_MATERIAL_COUNT; // FIXME: Can overwrite materials.

  return element;
}

void textures_material_element_destroy(textures_material_element *element) { core_free(element); }

textures *textures_create(device *vkd) {
  textures *textures = core_alloc(sizeof(struct textures));

  textures->vkd = vkd;
  textures->textureElements = NULL;
  textures->materialElements = NULL;

  return textures;
}

void textures_destroy(textures *textures) {
  uthash_foreach_elem_it(textures_texture_element *, element, textures->textureElements) {
    HASH_DEL(textures->textureElements, element);
    textures_texture_element_destroy(element);
  }
  uthash_foreach_elem_it(textures_material_element *, element, textures->materialElements) {
    HASH_DEL(textures->materialElements, element);
    textures_material_element_destroy(element);
  }

  core_free(textures);
}

void textures_update(textures *textures) {
  // No-op.
}

void textures_send_to_device(textures *textures) {
  uthash_foreach_elem_it(textures_texture_element *, element, textures->textureElements) {
    image_send_to_device(element->image);
  }
}

textures_material_element *textures_add_material(textures *textures, asset_material *material) {
  textures_material_element *element;
  HASH_FIND(hh, textures->materialElements, &material, sizeof(material), element);
  if (element != NULL) {
    return element;
  }

  log_info("adding material");
  asset_material_debug_print(material, 0);
  element = textures_material_element_create(
      material, textures_add_texture(textures, material->baseColorTexture, "material"),
      textures_add_texture(textures, material->metallicRoughnessTexture, "material"),
      textures_add_texture(textures, material->normalMapTexture, "material"));
  HASH_ADD_PTR(textures->materialElements, material, element);
  return element;
}

textures_texture_element *textures_add_texture(textures *textures, asset_texture *texture,
                                               const char *debugName) {
  textures_texture_element *element;
  HASH_FIND(hh, textures->textureElements, &texture, sizeof(texture), element);
  if (element != NULL) {
    return element;
  }

  log_info("adding texture");
  asset_texture_debug_print(texture, 0);
  element = textures_texture_element_create(texture, textures->vkd, debugName);
  HASH_ADD_PTR(textures->textureElements, texture, element);
  return element;
}

void textures_debug_print(textures *textures, int indent) {
  log_debug(INDENT_FORMAT_STRING "textures:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "texture count=%zu\n", INDENT_FORMAT_ARGS(2),
            HASH_CNT(hh, textures->textureElements));
  log_debug(INDENT_FORMAT_STRING "material count=%zu\n", INDENT_FORMAT_ARGS(2),
            HASH_CNT(hh, textures->materialElements));
}

void glsl_add_textures(UT_string *s, uint32_t set, uint32_t binding, textures *textures) {
  utstring_printf(s, "// Declare multiple shader variables with same set and binding numbers and "
                     "different shader types.\n"
                     "// Vulkan spec allows this aliasing ( \"DescriptorSet and Binding "
                     "Assignment\", https://github.com/KhronosGroup/Vulkan-Docs/issues/1420 ).\n");
  utstring_printf(s, "layout(set = %u, binding = %u) uniform sampler2D textures2D[];\n", set,
                  binding);
  utstring_printf(s, "layout(set = %u, binding = %u) uniform samplerCube texturesCube[];\n", set,
                  binding);
}
