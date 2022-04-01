#include "textures.h"
#include "../data/primitive.h"

vulkan_textures_element *vulkan_textures_element_create(vulkan_data_texture *texture,
                                                        vulkan_device *vkd) {
  vulkan_textures_element *element = core_alloc(sizeof(vulkan_textures_element));

  element->texture = texture;
  // HIRO HIRO format
  element->image =
      vulkan_image_create(vkd, vulkan_image_type_material_r8g8b8a8, element->texture->image->width,
                          element->texture->image->height);
  // HIRO HIRO sampler
  element->textureIdx = 0;

  return element;
}

void vulkan_textures_element_destroy(vulkan_textures_element *element) {
  vulkan_image_destroy(element->image);
  core_free(element);
}

vulkan_textures *vulkan_textures_create(vulkan_device *vkd,
                                        vulkan_render_cache_list *renderCacheList) {
  vulkan_textures *textures = core_alloc(sizeof(vulkan_textures));

  textures->vkd = vkd;
  textures->renderCacheList = renderCacheList;
  textures->elements = NULL;

  return textures;
}

void vulkan_textures_destroy(vulkan_textures *textures) {
  vulkan_textures_element *element, *temp;
  HASH_ITER(hh, textures->elements, element, temp) {
    HASH_DEL(textures->elements, element);
    vulkan_textures_element_destroy(element);
  }

  core_free(textures);
}

void vulkan_textures_update(vulkan_textures *textures) {
  assert(utarray_len(textures->renderCacheList->primitiveRenderCaches) > 0);

  utarray_foreach_elem_deref (vulkan_render_cache *, renderCache,
                              textures->renderCacheList->primitiveRenderCaches) {
    vulkan_data_material *material = renderCache->primitive->material;
    if (material == NULL) {
      continue;
    }
    vulkan_textures_add_texture(textures, material->baseColorTexture);
    vulkan_textures_add_texture(textures, material->metallicRoughnessTexture);

    // TODO: Unload unneeded textures.
  }
}

void vulkan_textures_send_to_device(vulkan_textures *textures) {
  vulkan_textures_element *element, *temp;
  HASH_ITER(hh, textures->elements, element, temp) { vulkan_image_send_to_device(element->image); }
}

void vulkan_textures_add_texture(vulkan_textures *textures, vulkan_data_texture *texture) {
  vulkan_textures_element *element;
  HASH_FIND(hh, textures->elements, &texture, sizeof(texture), element);
  if (element != NULL) {
    return;
  }

  log_info("adding texture");
  vulkan_data_texture_debug_print(texture);
  element = vulkan_textures_element_create(texture, textures->vkd);
  HASH_ADD_PTR(textures->elements, texture, element);
}

void vulkan_textures_debug_print(vulkan_textures *textures) {
  log_debug("textures:\n");
  log_debug("count=%zu\n", HASH_CNT(hh, textures->elements));
}
