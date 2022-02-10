/* Common scene data.
 */
#pragma once

#include "../../data/data.h"
#include "../functions.h"

typedef struct vulkan_data_scene vulkan_data_scene;
typedef struct vulkan_scene_node vulkan_scene_node;

#define DECL_SCENE_DATA(_name)                                                                     \
  data_key hash;                                                                                   \
  vulkan_scene_node *sceneGraphNode;                                                               \
  struct vulkan_data_##_name *prev, *next;

#define DEF_SCENE_DATA(_name)                                                                      \
  (_name)->hash = vulkan_data_##_name##_calculate_key(_name);                                      \
  (_name)->sceneGraphNode = NULL; /* set by scene graph when adding scene node*/                   \
  (_name)->prev = NULL;                                                                            \
  (_name)->next = NULL;
