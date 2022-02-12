/* Vulkan scene node.
 * Contains pointers to entities in scene data and cache.
 * Used to:
 * build scene graph with nodes corresponding 1:1 to scene data,
 * build scene tree with nodes from scene graph,
 * build render data with scene tree primitive nodes.
 */
#pragma once

#include "../data/scene.h"
#include "cache.h"

typedef struct vulkan_scene_graph vulkan_scene_graph;
typedef struct vulkan_scene_tree vulkan_scene_tree;

typedef enum vulkan_scene_node_type {
  vulkan_scene_node_type_root,
  vulkan_scene_node_type_object,
  vulkan_scene_node_type_mesh,
  vulkan_scene_node_type_primitive,
  vulkan_scene_node_type_count
} vulkan_scene_node_type;

// NOTE: There is no material node type - primitives are processed according to their materials
// only during draw call batching.

typedef struct vulkan_scene_node {
  vulkan_scene_node_type type;
  union {
    void *entity;
    vulkan_data_object *object;
    vulkan_data_mesh *mesh;
    vulkan_data_primitive *primitive;
  }; // pointer to entity in scene data

  vulkan_scene_graph *sceneGraph;
  vulkan_scene_tree *sceneTree;

  vulkan_scene_cache *cache; /// NULL for scene graph nodes.
  bool dirty;                /// True if scene node state changed and cache is out of sync.

  UT_array *parentNodes;      /// vulkan_scene_node* list
  UT_array *childObjectNodes; /// vulkan_scene_node* list
  vulkan_scene_node *meshNode;
  UT_array *primitiveNodes; /// vulkan_scene_node* list
  UT_array *observers;      /// vulkan_scene_node* list of dependent scene tree nodes

  struct vulkan_scene_node *prev, *next; /// Doubly-linked list of all nodes in scene graph/tree.
} vulkan_scene_node;

vulkan_scene_node *vulkan_scene_node_create(vulkan_scene_node_type type, void *entity,
                                            bool createCache);
void vulkan_scene_node_destroy(vulkan_scene_node *sceneNode);

void vulkan_scene_node_set_dirty(vulkan_scene_node *sceneNode);

void vulkan_scene_node_remove_util(vulkan_scene_node *sceneNode, vulkan_scene_node *nodes);

void vulkan_scene_node_debug_print(vulkan_scene_node *sceneNode);
