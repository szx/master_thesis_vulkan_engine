#include "graph.h"
#include "tree.h"

vulkan_scene_graph *vulkan_scene_graph_create(vulkan_data_scene *data) {
  vulkan_scene_graph *sceneGraph = core_alloc(sizeof(vulkan_scene_graph));
  sceneGraph->data = NULL;
  sceneGraph->sceneTree = vulkan_scene_tree_create(sceneGraph);
  sceneGraph->root = NULL;
  sceneGraph->nodes = NULL;

  vulkan_scene_graph_create_with_scene_data(sceneGraph, data);

  vulkan_scene_graph_set_dirty(sceneGraph, sceneGraph->root);
  vulkan_scene_tree_validate(sceneGraph->sceneTree);

  return sceneGraph;
}

void vulkan_scene_graph_destroy(vulkan_scene_graph *sceneGraph) {
  vulkan_scene_tree_destroy(sceneGraph->sceneTree);

  dl_foreach_elem (vulkan_scene_node *, node, sceneGraph->root) { vulkan_scene_node_destroy(node); }

  core_free(sceneGraph);
}

vulkan_scene_node *add_tree_node(vulkan_scene_tree *sceneTree, vulkan_scene_node *sceneGraphNode,
                                 vulkan_scene_node *parentSceneGraphNode,
                                 vulkan_scene_node *parentSceneTreeNode,
                                 vulkan_scene_node_entity_type type, void *entity) {
  assert(type != vulkan_scene_node_entity_type_root);
  assert(parentSceneGraphNode != NULL);

  vulkan_scene_node *sceneTreeNode =
      vulkan_scene_node_create_for_scene_tree(type, entity, sceneTree);
  DL_APPEND(sceneTree->nodes, sceneTreeNode);
  utarray_push_back(sceneGraphNode->observers, &sceneTreeNode);
  utarray_push_back(sceneTreeNode->parentNodes, &parentSceneTreeNode);

  if (sceneTreeNode->type == vulkan_scene_node_entity_type_object) {
    utarray_push_back(parentSceneTreeNode->childObjectNodes, &sceneTreeNode);
  } else if (sceneTreeNode->type == vulkan_scene_node_entity_type_mesh) {
    parentSceneTreeNode->meshNode = sceneTreeNode;
  } else if (sceneTreeNode->type == vulkan_scene_node_entity_type_primitive) {
    utarray_push_back(parentSceneTreeNode->primitiveNodes, &sceneTreeNode);
  } else {
    verify(0);
  }

  if (type == vulkan_scene_node_entity_type_object) {
    if (sceneGraphNode->meshNode) {
      vulkan_scene_node *childSceneTreeNode =
          add_tree_node(sceneTree, sceneGraphNode->meshNode, sceneGraphNode, sceneTreeNode,
                        vulkan_scene_node_entity_type_mesh, sceneGraphNode->mesh);
    }

    utarray_foreach_elem_deref (vulkan_scene_node *, child, sceneGraphNode->childObjectNodes) {
      vulkan_scene_node *childSceneTreeNode =
          add_tree_node(sceneTree, child, sceneGraphNode, sceneTreeNode,
                        vulkan_scene_node_entity_type_object, child->object);
    }
  } else if (type == vulkan_scene_node_entity_type_mesh) {
    utarray_foreach_elem_deref (vulkan_scene_node *, primitiveNode,
                                sceneGraphNode->primitiveNodes) {
      vulkan_scene_node *childSceneTreeNode =
          add_tree_node(sceneTree, primitiveNode, sceneGraphNode, sceneTreeNode,
                        vulkan_scene_node_entity_type_primitive, primitiveNode->primitive);
    }
  }

  return sceneTreeNode;
}

vulkan_scene_node *add_entity(vulkan_scene_graph *sceneGraph,
                              vulkan_scene_node *parentSceneGraphNode,
                              vulkan_scene_node_entity_type type, void *entity,
                              bool createSceneGraphNode) {
  /* add scene graph node */
  vulkan_scene_node *sceneGraphNode = NULL;

  bool uniqueSceneGraphNode = true;
  bool existsSceneGraphNode = false;

  if (uniqueSceneGraphNode) {
    dl_foreach_elem (vulkan_scene_node *, sceneNode, sceneGraph->root) {
      if (sceneNode->entity == entity) {
        // Return already added node.
        assert(sceneNode->type == type);
        sceneGraphNode = sceneNode;
        existsSceneGraphNode = true;
        break;
      }
    }
  }

  if (createSceneGraphNode && !existsSceneGraphNode) {
    sceneGraphNode = vulkan_scene_node_create_for_scene_graph(type, entity, sceneGraph);
    DL_APPEND(sceneGraph->nodes, sceneGraphNode);
  }
  assert(sceneGraphNode != NULL);

  if (parentSceneGraphNode != NULL) {
    assert(sceneGraphNode != parentSceneGraphNode);
    if (createSceneGraphNode) {
      utarray_push_back(sceneGraphNode->parentNodes, &parentSceneGraphNode);
      if (sceneGraphNode->type == vulkan_scene_node_entity_type_object) {
        utarray_push_back(parentSceneGraphNode->childObjectNodes, &sceneGraphNode);
      } else if (sceneGraphNode->type == vulkan_scene_node_entity_type_mesh) {
        parentSceneGraphNode->meshNode = sceneGraphNode;
      } else if (sceneGraphNode->type == vulkan_scene_node_entity_type_primitive) {
        utarray_push_back(parentSceneGraphNode->primitiveNodes, &sceneGraphNode);
      } else {
        verify(0);
      }
    }
  }

  /* add child graph nodes */
  if (type == vulkan_scene_node_entity_type_root) {
    vulkan_scene_node *sceneTreeNode =
        vulkan_scene_node_create_for_scene_tree(type, entity, sceneGraph->sceneTree);
    DL_APPEND(sceneGraph->sceneTree->nodes, sceneTreeNode);
    utarray_push_back(sceneGraphNode->observers, &sceneTreeNode);
  } else if (type == vulkan_scene_node_entity_type_object) {
    vulkan_data_object *object = entity;

    if (object->mesh) {
      vulkan_scene_node *childSceneGraphNode =
          add_entity(sceneGraph, sceneGraphNode, vulkan_scene_node_entity_type_mesh, object->mesh,
                     !existsSceneGraphNode);
    }

    utarray_foreach_elem_deref (vulkan_data_object *, child, object->children) {
      vulkan_scene_node *childSceneGraphNode =
          add_entity(sceneGraph, sceneGraphNode, vulkan_scene_node_entity_type_object, child,
                     !existsSceneGraphNode);
    }
  } else if (type == vulkan_scene_node_entity_type_mesh) {
    vulkan_data_mesh *mesh = entity;

    utarray_foreach_elem_deref (vulkan_data_primitive *, primitive, mesh->primitives) {
      vulkan_scene_node *childSceneGraphNode =
          add_entity(sceneGraph, sceneGraphNode, vulkan_scene_node_entity_type_primitive, primitive,
                     !existsSceneGraphNode);
    }
  }

  assert(sceneGraphNode != NULL);
  return sceneGraphNode;
}

void remove_entity(vulkan_scene_graph *sceneGraph, vulkan_scene_node *sceneGraphNode) {
  // FIXME: Simplify scene graph logic further - too many hoops to jump through right now.

  assert(sceneGraphNode->type == vulkan_scene_node_entity_type_object);
  vulkan_scene_node_remove_util(sceneGraphNode, sceneGraph->nodes);

  // remove observers and their successors
  utarray_foreach_elem_deref (vulkan_scene_node *, observerNode, sceneGraphNode->observers) {
    vulkan_scene_node *sceneTreeNode = *observerNodeIt;
    vulkan_scene_node_remove_util(sceneTreeNode, sceneGraph->sceneTree->nodes);
    vulkan_scene_node_destroy(sceneTreeNode);
  }

  // free
  vulkan_scene_node_destroy(sceneGraphNode);
}

vulkan_scene_node *vulkan_scene_graph_add_object(vulkan_scene_graph *sceneGraph,
                                                 vulkan_scene_node *sceneGraphNode,
                                                 vulkan_scene_node *sceneTreeNode,
                                                 vulkan_data_object *successorObject) {
  vulkan_scene_node *childSceneGraphNode = add_entity(
      sceneGraph, sceneGraphNode, vulkan_scene_node_entity_type_object, successorObject, true);

  vulkan_scene_node *childSceneTreeNode =
      add_tree_node(sceneGraph->sceneTree, childSceneGraphNode, sceneGraphNode, sceneTreeNode,
                    vulkan_scene_node_entity_type_object, successorObject);

  vulkan_scene_node_set_dirty(childSceneGraphNode);

  return childSceneGraphNode;
}

void vulkan_scene_graph_remove_object(vulkan_scene_graph *sceneGraph,
                                      vulkan_scene_node *sceneNode) {
  remove_entity(sceneGraph, sceneNode);
}

void vulkan_scene_graph_set_dirty(vulkan_scene_graph *sceneGraph, vulkan_scene_node *sceneNode) {
  utarray_foreach_elem_deref (vulkan_scene_node *, observerNode, sceneNode->observers) {
    vulkan_scene_tree_set_dirty(sceneGraph->sceneTree, observerNode);
  }
}

void vulkan_scene_graph_create_with_scene_data(vulkan_scene_graph *sceneGraph,
                                               vulkan_data_scene *sceneData) {
  sceneGraph->data = sceneData;
  assert(sceneGraph->root == NULL);
  assert(sceneGraph->nodes == NULL);

  sceneGraph->root = add_entity(sceneGraph, NULL, vulkan_scene_node_entity_type_root, NULL, true);
  sceneGraph->sceneTree->root = *(vulkan_scene_node **)utarray_front(sceneGraph->root->observers);

  utarray_foreach_elem_deref (vulkan_data_object *, rootObject, sceneGraph->data->rootObjects) {
    vulkan_scene_graph_add_object(sceneGraph, sceneGraph->root, sceneGraph->sceneTree->root,
                                  rootObject);
  }

  assert(sceneGraph->root);
}

void vulkan_scene_graph_debug_print(vulkan_scene_graph *sceneGraph) {
  log_raw(stdout, "digraph scene_graph { ");
  vulkan_scene_node_debug_print(sceneGraph->root);
  log_raw(stdout, " }\n");
}
