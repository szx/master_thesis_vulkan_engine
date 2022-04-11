#include "lights.h"
#include "descriptor.h"

vulkan_lights *vulkan_lights_create(vulkan_render_cache_list *renderCacheList) {
  vulkan_lights *lights = core_alloc(sizeof(vulkan_lights));

  lights->renderCacheList = renderCacheList;
  utarray_alloc(lights->lights, sizeof(vulkan_light_helper_element));
  vec3 position = GLM_VEC3_ZERO_INIT;
  vec3 color = GLM_VEC3_ONE_INIT;
  float radius = 1.0f;
  vulkan_lights_add(lights, position, color, radius);

  return lights;
}

void vulkan_lights_destroy(vulkan_lights *lights) { core_free(lights); }

void vulkan_lights_add(vulkan_lights *lights, vec3 position, vec3 color, float radius) {
  log_debug("adding light %zu at %f,%f,%f", utarray_len(lights->lights) + 1, position[0],
            position[1], position[2]);

  vulkan_light_helper_element element = {.radius = radius};
  glm_vec3_copy(position, element.position);
  glm_vec3_copy(color, element.color);
  utarray_push_back(lights->lights, &element);
  verify(utarray_len(lights->lights) <= MAX_LIGHT_COUNT);
}

void vulkan_lights_set_light_elements(vulkan_lights *lights, uint32_t *lightCount,
                                      vulkan_light_helper_element *lightElements) {
  *lightCount = utarray_len(lights->lights);
  core_memcpy(lightElements, utarray_front(lights->lights), utarray_size(lights->lights));
}

void vulkan_lights_debug_print(vulkan_lights *lights, int indent) {
  log_debug(INDENT_FORMAT_STRING "lights: count=%zu", INDENT_FORMAT_ARGS(0),
            utarray_len(lights->lights));
}
