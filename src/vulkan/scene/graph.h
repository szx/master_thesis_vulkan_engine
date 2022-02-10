/* Vulkan scene graph.
 * Contains scene nodes created from scene data and added/removed by user.
 * Used to build and synchronize scene tree.
 */
#pragma once

#include "node.h"

typedef struct vulkan_scene_graph {
  vulkan_data_scene *data; /// vulkan_data_scene pointer.

  vulkan_scene_node *root; /// Both root node and list of all nodes of scene graph.
} vulkan_scene_graph;

vulkan_scene_graph *vulkan_scene_graph_create(vulkan_data_scene *sceneData);
void vulkan_scene_graph_destroy(vulkan_scene_graph *sceneGraph);

vulkan_scene_node *vulkan_scene_graph_add_object(vulkan_scene_graph *sceneGraph,
                                                 vulkan_scene_node *sceneNode,
                                                 vulkan_data_object *successorObject);
vulkan_scene_node *vulkan_scene_graph_add_mesh(vulkan_scene_graph *sceneGraph,
                                               vulkan_scene_node *sceneNode,
                                               vulkan_data_mesh *successorMesh);
vulkan_scene_node *vulkan_scene_graph_add_primitive(vulkan_scene_graph *sceneGraph,
                                                    vulkan_scene_node *sceneNode,
                                                    vulkan_data_primitive *successorPrimitive);

void vulkan_scene_graph_create_with_scene_data(vulkan_scene_graph *sceneGraph,
                                               vulkan_data_scene *sceneData);

void vulkan_scene_graph_debug_print(vulkan_scene_graph *sceneGraph);