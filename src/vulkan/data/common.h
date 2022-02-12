/* Common scene data.
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"

typedef struct vulkan_data_scene vulkan_data_scene;
typedef struct vulkan_scene_node vulkan_scene_node;

#define DECL_VULKAN_ENTITY(_name)                                                                  \
  data_key hash;                                                                                   \
  vulkan_data_scene *sceneData;                                                                    \
  struct vulkan_data_##_name *prev, *next;

#define DEF_VULKAN_ENTITY(_name)                                                                   \
  (_name)->sceneData = sceneData;                                                                  \
  (_name)->hash = vulkan_data_##_name##_calculate_key(_name);                                      \
  (_name)->prev = NULL;                                                                            \
  (_name)->next = NULL;
