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

void add_object_data_as_child_scene_node(vulkan_scene_graph *sceneGraph,
                                         vulkan_scene_node *sceneNode, vulkan_data_object *object) {
  // HIRO currently we generate tree instead of graph, which is cool but insufficient
  // HIRO generate graph from scene data, same objects should result in same nodes
  vulkan_scene_node *childNode = vulkan_scene_node_create(vulkan_scene_node_type_object, object);
  DL_APPEND(sceneGraph->root, childNode);
  utarray_push_back(sceneNode->successors, &childNode);

  vulkan_data_object **childObject = NULL;
  while ((childObject = utarray_next(object->children, childObject))) {
    add_object_data_as_child_scene_node(sceneGraph, childNode, *childObject);
  }
}

void vulkan_scene_graph_create_with_scene_data(vulkan_scene_graph *sceneGraph,
                                               vulkan_data_scene *sceneData) {
  sceneGraph->data = sceneData;
  assert(sceneGraph->root == NULL);

  sceneGraph->rootObject = core_alloc(sizeof(vulkan_data_object));
  vulkan_data_object_init(sceneGraph->rootObject, sceneGraph->data);

  vulkan_scene_node *rootNode =
      vulkan_scene_node_create(vulkan_scene_node_type_object, sceneGraph->rootObject);
  DL_APPEND(sceneGraph->root, rootNode);

  vulkan_data_object *rootObject = NULL;
  while ((rootObject = utarray_next(sceneGraph->data->rootObjects, rootObject))) {
    utarray_push_back(sceneGraph->rootObject->children, &rootObject);
    add_object_data_as_child_scene_node(sceneGraph, sceneGraph->root, rootObject);
  }
  assert(sceneGraph->root);
}

void vulkan_scene_graph_debug_print(vulkan_scene_graph *sceneGraph) {
  log_debug("digraph scene_graph {");
  vulkan_scene_node_debug_print(sceneGraph->root);
  log_debug("}");
}
