#include "tree.h"

vulkan_scene_tree *vulkan_scene_tree_create(vulkan_scene_graph *sceneGraph) {
  vulkan_scene_tree *sceneTree = core_alloc(sizeof(vulkan_scene_tree));
  sceneTree->graph = NULL;
  sceneTree->root = NULL;
  vulkan_scene_tree_create_with_scene_graph(sceneTree, sceneGraph);
  return sceneTree;
}

void vulkan_scene_tree_destroy(vulkan_scene_tree *sceneTree) {
  vulkan_scene_node *node, *tempNode;
  DL_FOREACH_SAFE(sceneTree->root, node, tempNode) { vulkan_scene_node_destroy(node); }

  core_free(sceneTree);
}

vulkan_scene_node *add_graph_node_to_tree(vulkan_scene_tree *sceneTree,
                                          vulkan_scene_node *graphNode) {
  vulkan_scene_node *treeNode = vulkan_scene_node_create(graphNode->type, graphNode->entity);
  DL_APPEND(sceneTree->root, treeNode);

  vulkan_scene_node_add_observer(graphNode, treeNode);

  vulkan_scene_node **successorIt = NULL;
  while ((successorIt = utarray_next(graphNode->successors, successorIt))) {
    vulkan_scene_node *successor = *successorIt;
    vulkan_scene_node *childTreeNode = add_graph_node_to_tree(sceneTree, successor);
    vulkan_scene_node_add_successor(treeNode, childTreeNode);
  }

  return treeNode;
}

void vulkan_scene_tree_create_with_scene_graph(vulkan_scene_tree *sceneTree,
                                               vulkan_scene_graph *sceneGraph) {
  sceneTree->graph = sceneGraph;
  assert(sceneTree->root == NULL);

  add_graph_node_to_tree(sceneTree, sceneGraph->root);

  assert(sceneTree->root);
}

void vulkan_scene_tree_debug_print(vulkan_scene_tree *sceneTree) {
  log_raw(stdout, "digraph scene_tree {");
  vulkan_scene_node_debug_print(sceneTree->root);
  log_raw(stdout, "}\n");
}
