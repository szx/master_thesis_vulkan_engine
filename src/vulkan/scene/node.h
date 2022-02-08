/* Vulkan scene node.
 * Contains pointers to entities in scene data
 * Used to:
 * build scene graph with nodes corresponding 1:1 to scene data
 * build scene tree with nodes either from scene graph
 */
#pragma once

#include "../data/scene.h"

typedef enum vulkan_scene_node_type {
  vulkan_scene_node_type_object,
  vulkan_scene_node_type_mesh,
  vulkan_scene_node_type_primitive,
  vulkan_scene_node_type_count
} vulkan_scene_node_type;

typedef struct vulkan_scene_node {
  vulkan_scene_node_type type;
  union {
    vulkan_data_object *object;
    vulkan_data_mesh *mesh;
    vulkan_data_primitive *primitive;
  }; // pointer to entity in scene data

  struct vulkan_scene_node *next;
} vulkan_scene_node;

vulkan_scene_node *vulkan_scene_node_create(vulkan_scene_node_type type, void *entity);
void vulkan_scene_node_destroy(vulkan_scene_node *sceneNode);
