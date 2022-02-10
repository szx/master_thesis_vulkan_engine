#include "tree.h"

vulkan_scene_tree *vulkan_scene_tree_create(vulkan_scene_graph *sceneGraph) {
  vulkan_scene_tree *sceneTree = core_alloc(sizeof(vulkan_scene_tree));
  sceneTree->graph = NULL;
  sceneTree->root = NULL;
  sceneTree->nodes = NULL;
  utarray_alloc(sceneTree->dirtyNodes, sizeof(vulkan_scene_node *));
  vulkan_scene_tree_create_with_scene_graph(sceneTree, sceneGraph);
  return sceneTree;
}

void vulkan_scene_tree_destroy(vulkan_scene_tree *sceneTree) {
  utarray_free(sceneTree->dirtyNodes);

  vulkan_scene_node *node, *tempNode;
  DL_FOREACH_SAFE(sceneTree->nodes, node, tempNode) { vulkan_scene_node_destroy(node); }

  core_free(sceneTree);
}

void vulkan_scene_tree_set_dirty(vulkan_scene_tree *sceneTree, vulkan_scene_node *sceneNode) {
  if (!sceneNode->dirty) {
    sceneNode->dirty = true;
    utarray_push_back(sceneTree->dirtyNodes, &sceneNode);
  }
}

vulkan_scene_node *find_top_dirty_node(vulkan_scene_node *dirtyNode) {
  vulkan_scene_node *topNode = dirtyNode;
  while (topNode != NULL) {
    if (topNode->dirty) {
      dirtyNode = topNode;
    }
    topNode = topNode->parent;
  }
  return dirtyNode;
}

void validate_node(vulkan_scene_node *node) {
  if (node->dirty) {
    node->dirty = false;
  }

  if (node->parent) {
    vulkan_scene_cache_accumulate(node->cache, node->parent->cache);
  }

  vulkan_scene_node **successorIt = NULL;
  while ((successorIt = utarray_next(node->successors, successorIt))) {
    vulkan_scene_node *successor = *successorIt;
    validate_node(successor);
  }
}

void vulkan_scene_tree_validate(vulkan_scene_tree *sceneTree) {
  vulkan_scene_node **dirtyNodeIt = NULL;
  while ((dirtyNodeIt = utarray_next(sceneTree->dirtyNodes, dirtyNodeIt))) {
    vulkan_scene_node *dirtyNode = *dirtyNodeIt;
    if (dirtyNode->dirty) {
      vulkan_scene_node *topDirtyNode = find_top_dirty_node(dirtyNode);
      validate_node(topDirtyNode);
    }
  }
  utarray_clear(sceneTree->dirtyNodes);
}

vulkan_scene_node *add_node(vulkan_scene_tree *sceneTree, vulkan_scene_node_type type,
                            void *entity) {
  vulkan_scene_node *childNode = vulkan_scene_node_create(type, entity, true);
  DL_APPEND(sceneTree->nodes, childNode);
  return childNode;
}

vulkan_scene_node *add_graph_node_to_tree(vulkan_scene_tree *sceneTree,
                                          vulkan_scene_node *graphNode) {
  if (utarray_len(graphNode->successors) == 0) {
    if (graphNode->type == vulkan_scene_node_type_primitive) {
      // Found valid leaf.
      vulkan_scene_node *treeNode = add_node(sceneTree, graphNode->type, graphNode->entity);
      vulkan_scene_node_add_observer(graphNode, treeNode);
      return treeNode;
    }
    // Found invalid leaf, skip whole branch.
    return NULL;
  }

  vulkan_scene_node *treeNode = NULL;

  vulkan_scene_node **successorIt = NULL;
  while ((successorIt = utarray_next(graphNode->successors, successorIt))) {
    vulkan_scene_node *successor = *successorIt;
    vulkan_scene_node *childTreeNode = add_graph_node_to_tree(sceneTree, successor);
    if (childTreeNode != NULL) {
      if (treeNode == NULL) {
        treeNode = add_node(sceneTree, graphNode->type, graphNode->entity);
        vulkan_scene_node_add_observer(graphNode, treeNode);
      }
      vulkan_scene_node_add_successor(treeNode, childTreeNode);
    }
  }

  return treeNode;
}

void vulkan_scene_tree_create_with_scene_graph(vulkan_scene_tree *sceneTree,
                                               vulkan_scene_graph *sceneGraph) {
  sceneTree->graph = sceneGraph;
  sceneTree->graph->tree = sceneTree;
  assert(sceneTree->root == NULL && sceneTree->nodes == NULL);

  sceneTree->root = add_graph_node_to_tree(sceneTree, sceneGraph->root);
  vulkan_scene_graph_set_dirty(sceneGraph, sceneGraph->root);
  vulkan_scene_tree_validate(sceneTree);

  assert(sceneTree->root && sceneTree->nodes);
}

void vulkan_scene_tree_debug_print(vulkan_scene_tree *sceneTree) {
  log_raw(stdout, "digraph scene_tree {");
  vulkan_scene_node_debug_print(sceneTree->root);
  log_raw(stdout, "}\n");
}
