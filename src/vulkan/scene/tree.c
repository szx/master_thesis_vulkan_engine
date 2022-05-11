#include "tree.h"
#include "../renderers/render_cache_list.h"

vulkan_scene_tree *vulkan_scene_tree_create(vulkan_scene_graph *sceneGraph,
                                            vulkan_render_cache_list *renderCacheList) {
  vulkan_scene_tree *sceneTree = core_alloc(sizeof(vulkan_scene_tree));
  sceneTree->graph = sceneGraph;
  sceneTree->renderCacheList = renderCacheList;
  sceneTree->root = NULL;
  sceneTree->nodes = NULL;
  utarray_alloc(sceneTree->dirtyNodes, sizeof(vulkan_scene_tree_node *));
  return sceneTree;
}

void vulkan_scene_tree_destroy(vulkan_scene_tree *sceneTree) {
  utarray_free(sceneTree->dirtyNodes);

  dl_foreach_elem(vulkan_scene_tree_node *, node, sceneTree->nodes) {
    vulkan_scene_tree_node_destroy(node);
  }

  core_free(sceneTree);
}

vulkan_scene_tree_node *vulkan_scene_tree_add_node(vulkan_scene_tree *sceneTree,
                                                   vulkan_scene_graph_node *sceneGraphNode,
                                                   vulkan_scene_graph_node *parentSceneGraphNode,
                                                   vulkan_scene_tree_node *parentSceneTreeNode) {
  assert(parentSceneGraphNode != NULL);

  vulkan_scene_tree_node *sceneTreeNode = vulkan_scene_tree_node_create(sceneTree, sceneGraphNode);
  DL_APPEND(sceneTree->nodes, sceneTreeNode);
  vulkan_scene_tree_node_add_parent(sceneTreeNode, parentSceneTreeNode);

  vulkan_scene_graph_node_add_observer(sceneGraphNode, sceneTreeNode);

  if (sceneTreeNode->primitive != NULL) {
    vulkan_render_cache_list_add_primitive_render_cache(sceneTree->renderCacheList,
                                                        sceneTreeNode->renderCache);
  }
  if (sceneTreeNode->object != NULL && sceneTreeNode->object->camera != NULL) {
    vulkan_render_cache_list_add_camera_render_cache(sceneTree->renderCacheList,
                                                     sceneTreeNode->renderCache);
  }

  vulkan_scene_tree_node_add_child(parentSceneTreeNode, sceneTreeNode);

  if (sceneTreeNode->object != NULL) {
    utarray_foreach_elem_deref (vulkan_scene_graph_node *, childSceneGraphNode,
                                sceneGraphNode->childNodes) {
      vulkan_scene_tree_node *childSceneTreeNode =
          vulkan_scene_tree_add_node(sceneTree, childSceneGraphNode, sceneGraphNode, sceneTreeNode);
    }
  }

  return sceneTreeNode;
}

void vulkan_scene_tree_set_dirty(vulkan_scene_tree *sceneTree,
                                 vulkan_scene_tree_node *sceneTreeNode) {
  if (!sceneTreeNode->dirty) {
    sceneTreeNode->dirty = true;
    // Update render cache with dirty tree node.
    vulkan_scene_tree_node_set_render_cache(sceneTreeNode, sceneTreeNode->renderCache);
    utarray_push_back(sceneTree->dirtyNodes, &sceneTreeNode);
  }
}

vulkan_scene_tree_node *find_top_dirty_node(vulkan_scene_tree_node *dirtyNode) {
  vulkan_scene_tree_node *topNode = dirtyNode;
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

void validate_node(vulkan_scene_tree_node *node) {
  if (node->dirty) {
    node->dirty = false;
  }

  if (node->parentNode != NULL) {
    vulkan_scene_tree_node_accumulate_to_render_cache(node->parentNode, node->renderCache);
  }

  utarray_foreach_elem_deref (vulkan_scene_tree_node *, childNode, node->childNodes) {
    validate_node(childNode);
  }
}

void vulkan_scene_tree_validate(vulkan_scene_tree *sceneTree) {
  utarray_foreach_elem_deref (vulkan_scene_tree_node *, dirtyNode, sceneTree->dirtyNodes) {
    if (dirtyNode->dirty) {
      vulkan_scene_tree_node *topDirtyNode = find_top_dirty_node(dirtyNode);
      validate_node(topDirtyNode);
    }
  }
  utarray_clear(sceneTree->dirtyNodes);
}

void vulkan_scene_tree_debug_print(vulkan_scene_tree *sceneTree) {
  log_raw(stdout, "digraph scene_tree {");
  vulkan_scene_tree_node_debug_print(sceneTree->root);
  log_raw(stdout, "}\n");
  vulkan_render_cache_list_debug_print(sceneTree->renderCacheList);
}
