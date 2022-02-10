#include "graph.h"

vulkan_scene_graph *vulkan_scene_graph_create(vulkan_data_scene *data) {
  vulkan_scene_graph *sceneGraph = core_alloc(sizeof(vulkan_scene_graph));
  sceneGraph->data = NULL;
  sceneGraph->root = NULL;
  vulkan_scene_graph_create_with_scene_data(sceneGraph, data);
  return sceneGraph;
}

void vulkan_scene_graph_destroy(vulkan_scene_graph *sceneGraph) {
  vulkan_scene_node *node, *tempNode;
  DL_FOREACH_SAFE(sceneGraph->root, node, tempNode) { vulkan_scene_node_destroy(node); }

  core_free(sceneGraph);
}

vulkan_scene_node *add_entity(vulkan_scene_graph *sceneGraph, vulkan_scene_node_type type,
                              void *entity) {
  vulkan_scene_node *sceneNode = NULL;
  DL_FOREACH(sceneGraph->root, sceneNode) {
    if (sceneNode->entity == entity) {
      // Return already added node.
      assert(sceneNode->type == type);
      return sceneNode;
    }
  }
  vulkan_scene_node *childNode = vulkan_scene_node_create(type, entity, false);
  DL_APPEND(sceneGraph->root, childNode);
  return childNode;
}

vulkan_scene_node *vulkan_scene_graph_add_object(vulkan_scene_graph *sceneGraph,
                                                 vulkan_scene_node *sceneNode,
                                                 vulkan_data_object *successorObject) {
  vulkan_scene_node *childNode =
      add_entity(sceneGraph, vulkan_scene_node_type_object, successorObject);
  vulkan_scene_node_add_successor(sceneNode, childNode);

  if (successorObject->mesh) {
    vulkan_scene_graph_add_mesh(sceneGraph, childNode, successorObject->mesh);
  }

  vulkan_data_object **childObject = NULL;
  while ((childObject = utarray_next(successorObject->children, childObject))) {
    vulkan_scene_graph_add_object(sceneGraph, childNode, *childObject);
  }

  return childNode;
}

vulkan_scene_node *vulkan_scene_graph_add_mesh(vulkan_scene_graph *sceneGraph,
                                               vulkan_scene_node *sceneNode,
                                               vulkan_data_mesh *successorMesh) {
  vulkan_scene_node *childNode = add_entity(sceneGraph, vulkan_scene_node_type_mesh, successorMesh);
  vulkan_scene_node_add_successor(sceneNode, childNode);

  vulkan_data_primitive **primitiveIt = NULL;
  while ((primitiveIt = (utarray_next(successorMesh->primitives, primitiveIt)))) {
    vulkan_scene_graph_add_primitive(sceneGraph, childNode, *primitiveIt);
  }

  return childNode;
}

vulkan_scene_node *vulkan_scene_graph_add_primitive(vulkan_scene_graph *sceneGraph,
                                                    vulkan_scene_node *sceneNode,
                                                    vulkan_data_primitive *successorPrimitive) {
  vulkan_scene_node *childNode =
      add_entity(sceneGraph, vulkan_scene_node_type_primitive, successorPrimitive);
  vulkan_scene_node_add_successor(sceneNode, childNode);
  return childNode;
}

void vulkan_scene_graph_create_with_scene_data(vulkan_scene_graph *sceneGraph,
                                               vulkan_data_scene *sceneData) {
  sceneGraph->data = sceneData;
  assert(sceneGraph->root == NULL);

  vulkan_scene_node *rootNode = vulkan_scene_node_create(vulkan_scene_node_type_root, NULL, false);
  DL_APPEND(sceneGraph->root, rootNode);

  vulkan_data_object *rootObject = NULL;
  while ((rootObject = utarray_next(sceneGraph->data->rootObjects, rootObject))) {
    vulkan_scene_graph_add_object(sceneGraph, sceneGraph->root, rootObject);
  }
  assert(sceneGraph->root);
}

void vulkan_scene_graph_debug_print(vulkan_scene_graph *sceneGraph) {
  log_raw(stdout, "digraph scene_graph { ");
  vulkan_scene_node_debug_print(sceneGraph->root);
  log_raw(stdout, " }\n");
}
