#include "tree.h"

vulkan_scene_tree *vulkan_scene_tree_create(vulkan_data_scene *data) {
  vulkan_scene_tree *sceneGraph = core_alloc(sizeof(vulkan_scene_tree));
  sceneGraph->root = NULL;
  vulkan_scene_tree_create_with_scene_data(sceneGraph, data);
  return sceneGraph;
}

void vulkan_scene_tree_destroy(vulkan_scene_tree *sceneGraph) {
  vulkan_scene_node *node, *tempNode;
  DL_FOREACH_SAFE(sceneGraph->root, node, tempNode) { vulkan_scene_node_destroy(node); }

  vulkan_data_object_deinit(sceneGraph->rootObject);
  core_free(sceneGraph->rootObject);

  core_free(sceneGraph);
}

void vulkan_scene_tree_create_with_scene_data(vulkan_scene_tree *sceneGraph,
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
    // add_object_data_as_child_scene_node(sceneGraph, sceneGraph->root, rootObject);
  }
  assert(sceneGraph->root);
}

void vulkan_scene_tree_debug_print(vulkan_scene_tree *sceneGraph) {
  log_debug("digraph scene_tree {");
  vulkan_scene_node_debug_print(sceneGraph->root);
  log_debug("}");
}
