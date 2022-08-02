#include "tree.h"
#include "../renderers/renderer_cache.h"

scene_tree *scene_tree_create(scene_graph *sceneGraph, renderer_cache *rendererCache) {
  scene_tree *sceneTree = core_alloc(sizeof(scene_tree));
  sceneTree->graph = sceneGraph;
  sceneTree->rendererCache = rendererCache;
  sceneTree->root = NULL;
  sceneTree->nodes = NULL;
  utarray_alloc(sceneTree->dirtyNodes, sizeof(scene_tree_node *));
  return sceneTree;
}

void scene_tree_destroy(scene_tree *sceneTree) {
  utarray_free(sceneTree->dirtyNodes);

  dl_foreach_elem(scene_tree_node *, node, sceneTree->nodes) { scene_tree_node_destroy(node); }

  core_free(sceneTree);
}

scene_tree_node *scene_tree_add_node(scene_tree *sceneTree, scene_graph_node *sceneGraphNode,
                                     scene_graph_node *parentSceneGraphNode,
                                     scene_tree_node *parentSceneTreeNode) {
  assert(parentSceneGraphNode != NULL);

  scene_tree_node *sceneTreeNode = scene_tree_node_create(sceneTree, sceneGraphNode);
  DL_APPEND(sceneTree->nodes, sceneTreeNode);
  scene_tree_node_add_parent(sceneTreeNode, parentSceneTreeNode);

  scene_graph_node_add_observer(sceneGraphNode, sceneTreeNode);

  scene_tree_node_add_child(parentSceneTreeNode, sceneTreeNode);

  if (sceneTreeNode->object != NULL) {
    utarray_foreach_elem_deref (scene_graph_node *, childSceneGraphNode,
                                sceneGraphNode->childNodes) {
      scene_tree_node *childSceneTreeNode =
          scene_tree_add_node(sceneTree, childSceneGraphNode, sceneGraphNode, sceneTreeNode);
    }
  }

  return sceneTreeNode;
}

void scene_tree_set_dirty(scene_tree *sceneTree, scene_tree_node *sceneTreeNode) {
  if (!sceneTreeNode->dirty) {
    sceneTreeNode->dirty = true;
    // Update if dirty because assets has been changed.
    scene_tree_node_reset_accumulated(sceneTreeNode);
    utarray_push_back(sceneTree->dirtyNodes, &sceneTreeNode);
  }
}

scene_tree_node *find_top_dirty_node(scene_tree_node *dirtyNode) {
  scene_tree_node *topNode = dirtyNode;
  while (true) {
    if (topNode->dirty) {
      dirtyNode = topNode;
    }
    if (topNode->parentNode == NULL) {
      break;
    }
    topNode = topNode->parentNode;
  }
  return dirtyNode;
}

void validate_node(scene_tree_node *node) {
  if (node->dirty) {
    node->dirty = false;
  }

  if (node->parentNode != NULL) {
    scene_tree_node_accumulate_from_parent(node);
  }

  utarray_foreach_elem_deref (scene_tree_node *, childNode, node->childNodes) {
    validate_node(childNode);
  }
}

void scene_tree_validate(scene_tree *sceneTree) {
  utarray_foreach_elem_deref (scene_tree_node *, dirtyNode, sceneTree->dirtyNodes) {
    if (dirtyNode->dirty) {
      scene_tree_node *topDirtyNode = find_top_dirty_node(dirtyNode);
      validate_node(topDirtyNode);
    }
  }
  utarray_clear(sceneTree->dirtyNodes);
}

void scene_tree_debug_print(scene_tree *sceneTree) {
  log_raw(stdout, "digraph scene_tree {");
  scene_tree_node_debug_print(sceneTree->root);
  log_raw(stdout, "}\n");
  renderer_cache_debug_print(sceneTree->rendererCache);
}
