#include "graph.h"

vulkan_scene_graph *vulkan_scene_graph_create(vulkan_data_scene *data) {
  vulkan_scene_graph *sceneGraph = core_alloc(sizeof(vulkan_scene_graph));
  sceneGraph->root = NULL;
  vulkan_scene_graph_create_with_scene_data(sceneGraph, data);
  return sceneGraph;
}

void vulkan_scene_graph_destroy(vulkan_scene_graph *sceneGraph) {
  vulkan_scene_node *node, *tempNode;
  DL_FOREACH_SAFE(sceneGraph->root, node, tempNode) { vulkan_scene_node_destroy(node); }

  vulkan_data_object_deinit(sceneGraph->rootObject);
  core_free(sceneGraph->rootObject);

  core_free(sceneGraph);
}

void vulkan_scene_graph_create_with_scene_data(vulkan_scene_graph *sceneGraph,
                                               vulkan_data_scene *sceneData) {
  sceneGraph->data = sceneData;
  assert(sceneGraph->root == NULL);
  sceneGraph->rootObject = core_alloc(sizeof(vulkan_data_object));
  vulkan_data_object_init(sceneGraph->rootObject, sceneGraph->data);
  sceneGraph->root =
      vulkan_scene_node_create(vulkan_scene_node_type_object, sceneGraph->rootObject);
  // HIRO create nodes from scene data and add them to root linked list.
  assert(sceneGraph->root);
}

void vulkan_scene_graph_debug_print(vulkan_scene_graph *sceneGraph) {
  log_debug("SCENE GRAPH:\n");
  vulkan_scene_node_debug_print(sceneGraph->root);
}
