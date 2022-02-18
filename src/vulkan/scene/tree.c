#include "tree.h"
#include "cache_list.h"

vulkan_scene_tree *vulkan_scene_tree_create(vulkan_scene_graph *sceneGraph) {
  vulkan_scene_tree *sceneTree = core_alloc(sizeof(vulkan_scene_tree));
  sceneTree->graph = sceneGraph;
  sceneTree->cacheList = vulkan_scene_cache_list_create(sceneTree);
  sceneTree->root = NULL;
  sceneTree->nodes = NULL;
  utarray_alloc(sceneTree->dirtyNodes, sizeof(vulkan_scene_node *));
  return sceneTree;
}

void vulkan_scene_tree_destroy(vulkan_scene_tree *sceneTree) {
  utarray_free(sceneTree->dirtyNodes);

  dl_foreach_elem(vulkan_scene_node *, node, sceneTree->nodes, { vulkan_scene_node_destroy(node); })

  vulkan_scene_cache_list_destroy(sceneTree->cacheList);

  core_free(sceneTree);
}

vulkan_scene_node *vulkan_scene_tree_add_node(vulkan_scene_tree *sceneTree,
                                              vulkan_scene_node *sceneGraphNode,
                                              vulkan_scene_node *parentSceneGraphNode,
                                              vulkan_scene_node *parentSceneTreeNode,
                                              vulkan_scene_node_entity_type type, void *entity) {
  assert(type != vulkan_scene_node_entity_type_root);
  assert(parentSceneGraphNode != NULL);

  vulkan_scene_node *sceneTreeNode =
      vulkan_scene_node_create_for_scene_tree(type, entity, sceneTree);
  DL_APPEND(sceneTree->nodes, sceneTreeNode);
  utarray_push_back(sceneTreeNode->parentNodes, &parentSceneTreeNode);

  utarray_push_back(sceneGraphNode->observers, &sceneTreeNode);
  if (type == vulkan_scene_node_entity_type_primitive) {
    vulkan_scene_cache_list_add_cache(sceneTree->cacheList, sceneTreeNode->cache);
  }

  if (sceneTreeNode->type == vulkan_scene_node_entity_type_object) {
    utarray_push_back(parentSceneTreeNode->childObjectNodes, &sceneTreeNode);
  } else if (sceneTreeNode->type == vulkan_scene_node_entity_type_primitive) {
    utarray_push_back(parentSceneTreeNode->primitiveNodes, &sceneTreeNode);
  } else {
    verify(0);
  }

  if (type == vulkan_scene_node_entity_type_object) {
    utarray_foreach_elem_deref (vulkan_scene_node *, primitiveNode,
                                sceneGraphNode->primitiveNodes) {
      vulkan_scene_node *childSceneTreeNode = vulkan_scene_tree_add_node(
          sceneTree, primitiveNode, sceneGraphNode, sceneTreeNode,
          vulkan_scene_node_entity_type_primitive, primitiveNode->primitive);
    }

    utarray_foreach_elem_deref (vulkan_scene_node *, child, sceneGraphNode->childObjectNodes) {
      vulkan_scene_node *childSceneTreeNode =
          vulkan_scene_tree_add_node(sceneTree, child, sceneGraphNode, sceneTreeNode,
                                     vulkan_scene_node_entity_type_object, child->object);
    }
  }

  return sceneTreeNode;
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
    if (topNode->type == vulkan_scene_node_entity_type_root) {
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
  vulkan_scene_cache_list_debug_print(sceneTree->cacheList);
}
