#include "textures.h"

vulkan_textures_element *vulkan_textures_element_create(vulkan_data_texture *texture,
                                                        vulkan_device *vkd) {
  vulkan_textures_element *element = core_alloc(sizeof(vulkan_textures_element));

  element->texture = texture;
  // NOTE: Assume that all materials are in RGBA 8-bit per-channel format.
  element->image =
      vulkan_image_create(vkd, vulkan_image_type_material_r8g8b8a8, element->texture->image->width,
                          element->texture->image->height);
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
    size_t instanceId = renderCache->renderCacheListIdx;
    // HIRO HIRO create vulkan_textures_element for each primitive's material's textures.
  }

  // TODO: Dirty only parts of unified uniform buffer.
}

void vulkan_textures_send_to_device(vulkan_textures *textures) {
  vulkan_textures_element *element, *temp;
  HASH_ITER(hh, textures->elements, element, temp) { vulkan_image_send_to_device(element->image); }
}

void vulkan_textures_debug_print(vulkan_textures *textures) {
  log_debug("textures:\n");
  log_debug("count=%zu\n", HASH_CNT(hh, textures->elements));
}
