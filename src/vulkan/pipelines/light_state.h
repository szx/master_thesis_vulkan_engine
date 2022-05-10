/* Vulkan lights.
 */
#pragma once

#include "../common.h"
#include "../objects/render_state.h"

typedef struct vulkan_pipeline_light_state {
  vulkan_render_state *renderState; ///< Pointer.

  UT_array *directionalLightElements; ///< vulkan_directional_light_helper_element list.
  UT_array *pointLightElements;       ///< vulkan_point_light_helper_element list.

} vulkan_pipeline_light_state;

vulkan_pipeline_light_state *vulkan_pipeline_light_state_create(vulkan_render_state *renderState);
void vulkan_pipeline_light_state_destroy(vulkan_pipeline_light_state *lights);

vulkan_directional_light_helper_element *
vulkan_pipeline_light_state_get_directional_light_element(vulkan_pipeline_light_state *lights,
                                                          size_t lightIdx);
void vulkan_pipeline_light_state_add_directional_light(vulkan_pipeline_light_state *lights,
                                                       vec3 direction, vec3 color);
void vulkan_pipeline_light_state_set_directional_light_elements(
    vulkan_pipeline_light_state *lights, uint32_t *directionalLightCount,
    vulkan_directional_light_helper_element *directionalLightElements);

vulkan_point_light_helper_element *
vulkan_pipeline_light_state_get_point_light_element(vulkan_pipeline_light_state *lights,
                                                    size_t lightIdx);
void vulkan_pipeline_light_state_add_point_light(vulkan_pipeline_light_state *lights, vec3 position,
                                                 vec3 color, float radius);
void vulkan_pipeline_light_state_set_point_light_elements(
    vulkan_pipeline_light_state *lights, uint32_t *pointLightCount,
    vulkan_point_light_helper_element *pointLightElements);

void vulkan_pipeline_light_state_debug_print(vulkan_pipeline_light_state *lights, int indent);
