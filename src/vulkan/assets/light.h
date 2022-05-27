/* Light state.
 */
#pragma once

#include "common.h"

// NOTE; Inspired by KHR_lights_punctual
typedef struct vulkan_asset_direct_light {
  vulkan_direct_light_type type;

  union {
    vec3 position;
    vec3 direction;
  };

  float innerConeAngle;
  float outerConeAngle;

  float intensity;
  float range;
  vec3 color;

  VULKAN_ASSET_FIELD_DECLS(direct_light)
} vulkan_asset_direct_light;

VULKAN_ASSET_FUNC_DECLS(direct_light)

vulkan_asset_direct_light *
vulkan_asset_direct_light_create_directional_light(vulkan_scene_data *sceneData, vec3 direction,
                                                   float intensity, vec3 color);

vulkan_asset_direct_light *
vulkan_asset_direct_light_create_point_light(vulkan_scene_data *sceneData, vec3 position,
                                             float intensity, float range, vec3 color);

vulkan_asset_direct_light *
vulkan_asset_direct_light_create_spot_light(vulkan_scene_data *sceneData, vec3 position,
                                            float innerConeAngle, float outerConeAngle,
                                            float intensity, float range, vec3 color);
