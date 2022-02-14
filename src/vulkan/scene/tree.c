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

void vulkan_scene_tree_set_dirty(vulkan_scene_tree *sceneTree, vulkan_scene_node *sceneNode) {
  if (!sceneNode->dirty) {
    sceneNode->dirty = true;
    utarray_push_back(sceneTree->dirtyNodes, &sceneNode);
  }
}

vulkan_scene_node *find_top_dirty_node(vulkan_scene_node *dirtyNode) {
  vulkan_scene_node *topNode = dirtyNode;
  while (true) {
    if (topNode->dirty) {
      dirtyNode = topNode;
    }
    if (topNode->type == vulkan_scene_node_type_root) {
      break;
    }
    assert(utarray_len(topNode->parentNodes) == 1);
    topNode = *(vulkan_scene_node **)utarray_front(topNode->parentNodes);
  }
  return dirtyNode;
}

void validate_node(vulkan_scene_node *node) {
  if (node->dirty) {
    node->dirty = false;
  }

  assert(utarray_len(node->parentNodes) == 0 || utarray_len(node->parentNodes) == 1);
  if (utarray_len(node->parentNodes) == 1) {
    vulkan_scene_node *parent = *(vulkan_scene_node **)utarray_front(node->parentNodes);
    vulkan_scene_cache_accumulate(node->cache, parent->cache);
  }

  utarray_foreach_elem_deref (vulkan_scene_node *, childObjectNode, node->childObjectNodes) {
    validate_node(childObjectNode);
  }

  if (node->meshNode != NULL) {
    validate_node(node->meshNode);
  }

  utarray_foreach_elem_deref (vulkan_scene_node *, primitiveNode, node->primitiveNodes) {
    validate_node(primitiveNode);
  }
}

void vulkan_scene_tree_validate(vulkan_scene_tree *sceneTree) {
  utarray_foreach_elem_deref (vulkan_scene_node *, dirtyNode, sceneTree->dirtyNodes) {
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
