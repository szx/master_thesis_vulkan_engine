/* Common scene data.
 */
#pragma once

#include "../../core/core.h"
#include "../../data/data.h"

typedef struct vulkan_data_scene vulkan_data_scene;
typedef struct vulkan_scene_node vulkan_scene_node;

#define DECL_VULKAN_ENTITY(_name)                                                                  \
  data_key key;                                                                                    \
  vulkan_data_scene *sceneData;                                                                    \
  struct vulkan_data_##_name *prev, *next;

#define DEF_VULKAN_ENTITY(_name, _var)                                                             \
  (_var)->sceneData = sceneData;                                                                   \
  (_var)->key = vulkan_data_##_name##_calculate_key(_var);                                         \
  (_var)->prev = NULL;                                                                             \
  (_var)->next = NULL;
