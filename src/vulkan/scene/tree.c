#include "tree.h"

vulkan_scene_tree *vulkan_scene_tree_create(vulkan_scene_graph *sceneGraph) {
  vulkan_scene_tree *sceneTree = core_alloc(sizeof(vulkan_scene_tree));
  sceneTree->graph = sceneGraph;
  sceneTree->root = NULL;
  sceneTree->nodes = NULL;
  utarray_alloc(sceneTree->dirtyNodes, sizeof(vulkan_scene_node *));
  return sceneTree;
}

void vulkan_scene_tree_destroy(vulkan_scene_tree *sceneTree) {
  utarray_free(sceneTree->dirtyNodes);

  vulkan_scene_node *node, *tempNode;
  DL_FOREACH_SAFE(sceneTree->nodes, node, tempNode) { vulkan_scene_node_destroy(node); }

  core_free(sceneTree);
}

vulkan_scene_node *add_tree_node_from_graph_node(vulkan_scene_tree *sceneTree,
                                                 vulkan_scene_node *sceneGraphNode) {
  vulkan_scene_node *treeNode =
      vulkan_scene_node_create(sceneGraphNode->type, sceneGraphNode->entity, true);
  DL_APPEND(sceneTree->nodes, treeNode);
  treeNode->sceneTree = sceneTree;
  vulkan_scene_node_add_observer(sceneGraphNode, treeNode);
  return treeNode;
}

vulkan_scene_node *vulkan_scene_tree_add_primitive_node(vulkan_scene_tree *sceneTree,
                                                        vulkan_scene_node *sceneGraphNode) {
  vulkan_scene_node *treeNode = add_tree_node_from_graph_node(sceneTree, sceneGraphNode);
  sceneGraphNode->primitive->sceneTreeNode = treeNode;

  vulkan_scene_node *currentTreeNode = treeNode;
  vulkan_scene_node *parentGraphNode = sceneGraphNode->parent;
  while (parentGraphNode != NULL) {
    if (parentGraphNode->type == vulkan_scene_node_type_root) {
      if (sceneTree->root == NULL) {
        sceneTree->root = add_tree_node_from_graph_node(sceneTree, sceneTree->graph->root);
      }
      vulkan_scene_node_add_successor(sceneTree->root, currentTreeNode);
      break;
    }

    if (parentGraphNode->type == vulkan_scene_node_type_mesh) {
      if (parentGraphNode->mesh->sceneTreeNode == NULL) {
        parentGraphNode->mesh->sceneTreeNode =
            add_tree_node_from_graph_node(sceneTree, parentGraphNode);
      }
      vulkan_scene_node *parentTreeNode = parentGraphNode->mesh->sceneTreeNode;
      vulkan_scene_node_add_successor(parentTreeNode, currentTreeNode);
      currentTreeNode = parentTreeNode;
    } else if (parentGraphNode->type == vulkan_scene_node_type_object) {
      if (parentGraphNode->object->sceneTreeNode == NULL) {
        parentGraphNode->object->sceneTreeNode =
            add_tree_node_from_graph_node(sceneTree, parentGraphNode);
      }
      vulkan_scene_node *parentTreeNode = parentGraphNode->object->sceneTreeNode;
      vulkan_scene_node_add_successor(parentTreeNode, currentTreeNode);
      currentTreeNode = parentTreeNode;
    }

    parentGraphNode = parentGraphNode->parent;
  }
  return treeNode;
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

void vulkan_scene_tree_debug_print(vulkan_scene_tree *sceneTree) {
  log_raw(stdout, "digraph scene_tree {");
  vulkan_scene_node_debug_print(sceneTree->root);
  log_raw(stdout, "}\n");
}
