/* Vulkan lights.
 */
#pragma once

#include "../common.h"
#include "render_cache_list.h"
typedef struct vulkan_light_helper_element vulkan_light_helper_element;

typedef struct vulkan_lights {
  vulkan_render_cache_list *renderCacheList; ///< Pointer.

  UT_array *pointLightElements; ///< vulkan_light_helper_element list.

} vulkan_lights;

vulkan_lights *vulkan_lights_create(vulkan_render_cache_list *renderCacheList);
void vulkan_lights_destroy(vulkan_lights *lights);

vulkan_light_helper_element *vulkan_lights_get_point_light_element(vulkan_lights *lights,
                                                                   size_t lightIdx);

void vulkan_lights_add_point_light(vulkan_lights *lights, vec3 position, vec3 color, float radius);

void vulkan_lights_set_point_light_elements(vulkan_lights *lights, uint32_t *pointLightCount,
                                            vulkan_light_helper_element *pointLightElements);

void vulkan_lights_debug_print(vulkan_lights *lights, int indent);