#include "light_state.h"

vulkan_pipeline_light_state *vulkan_pipeline_light_state_create(vulkan_render_state *renderState) {
  vulkan_pipeline_light_state *lights = core_alloc(sizeof(vulkan_pipeline_light_state));

  lights->renderState = renderState;
  utarray_alloc(lights->directionalLightElements, sizeof(vulkan_directional_light_helper_element));
  utarray_alloc(lights->pointLightElements, sizeof(vulkan_point_light_helper_element));

  return lights;
}

void vulkan_pipeline_light_state_destroy(vulkan_pipeline_light_state *lights) {
  utarray_free(lights->pointLightElements);
  utarray_free(lights->directionalLightElements);
  core_free(lights);
}

void vulkan_pipeline_light_state_reinit_with_new_swap_chain(vulkan_pipeline_light_state *light) {
  // No-op.
}

void vulkan_pipeline_light_state_update(vulkan_pipeline_light_state *light) {
  // No-op.
  // TODO: Update light positions?
}

vulkan_directional_light_helper_element *
vulkan_pipeline_light_state_get_directional_light_element(vulkan_pipeline_light_state *lights,
                                                          size_t lightIdx) {
  if (utarray_len(lights->directionalLightElements) == 0) {
    // NOTE: Default directional light.
    vec3 direction = {0.0, 0.0, 1.0};
    vec3 color = GLM_VEC3_ONE_INIT;
    vulkan_pipeline_light_state_add_directional_light(lights, direction, color);
    return vulkan_pipeline_light_state_get_directional_light_element(lights, lightIdx);
  }
  lightIdx = lightIdx % utarray_len(lights->directionalLightElements);
  return utarray_eltptr(lights->directionalLightElements, lightIdx);
}

void vulkan_pipeline_light_state_add_directional_light(vulkan_pipeline_light_state *lights,
                                                       vec3 direction, vec3 color) {
  log_debug("adding directional light %zu directed at %f,%f,%f",
            utarray_len(lights->directionalLightElements) + 1, direction[0], direction[1],
            direction[2]);

  vulkan_directional_light_helper_element element = {0};
  glm_vec3_copy(direction, element.direction);
  glm_vec3_copy(color, element.color);
  utarray_push_back(lights->directionalLightElements, &element);
  verify(utarray_len(lights->directionalLightElements) <= MAX_DIRECTIONAL_LIGHT_COUNT);
}

void vulkan_pipeline_light_state_set_directional_light_elements(
    vulkan_pipeline_light_state *lights, uint32_t *directionalLightCount,
    vulkan_directional_light_helper_element *directionalLightElements) {
  *directionalLightCount = utarray_len(lights->directionalLightElements);
  core_memcpy(directionalLightElements, utarray_front(lights->directionalLightElements),
              utarray_size(lights->directionalLightElements));
}

vulkan_point_light_helper_element *
vulkan_pipeline_light_state_get_point_light_element(vulkan_pipeline_light_state *lights,
                                                    size_t lightIdx) {
  if (utarray_len(lights->pointLightElements) == 0) {
    // NOTE: Default point light.
    vec3 position = GLM_VEC3_ZERO_INIT;
    vec3 color = GLM_VEC3_ONE_INIT;
    float radius = 1.0f;
    vulkan_pipeline_light_state_add_point_light(lights, position, color, radius);
    return vulkan_pipeline_light_state_get_point_light_element(lights, lightIdx);
  }
  lightIdx = lightIdx % utarray_len(lights->pointLightElements);
  return utarray_eltptr(lights->pointLightElements, lightIdx);
}

void vulkan_pipeline_light_state_add_point_light(vulkan_pipeline_light_state *lights, vec3 position,
                                                 vec3 color, float radius) {
  log_debug("adding point light %zu at %f,%f,%f", utarray_len(lights->pointLightElements) + 1,
            position[0], position[1], position[2]);

  vulkan_point_light_helper_element element = {.radius = radius};
  glm_vec3_copy(position, element.position);
  glm_vec3_copy(color, element.color);
  utarray_push_back(lights->pointLightElements, &element);
  verify(utarray_len(lights->pointLightElements) <= MAX_POINT_LIGHT_COUNT);
}

void vulkan_pipeline_light_state_set_point_light_elements(
    vulkan_pipeline_light_state *lights, uint32_t *pointLightCount,
    vulkan_point_light_helper_element *pointLightElements) {
  *pointLightCount = utarray_len(lights->pointLightElements);
  core_memcpy(pointLightElements, utarray_front(lights->pointLightElements),
              utarray_size(lights->pointLightElements));
}

void vulkan_pipeline_light_state_debug_print(vulkan_pipeline_light_state *lights, int indent) {
  log_debug(INDENT_FORMAT_STRING "lights:", INDENT_FORMAT_ARGS(0));
  log_debug(INDENT_FORMAT_STRING "directional light count=%zu", INDENT_FORMAT_ARGS(2),
            utarray_len(lights->directionalLightElements));
  log_debug(INDENT_FORMAT_STRING "point light count=%zu", INDENT_FORMAT_ARGS(2),
            utarray_len(lights->pointLightElements));
}
