#include "graph.h"
#include "tree.h"

vulkan_scene_graph *vulkan_scene_graph_create(vulkan_data_scene *data) {
  vulkan_scene_graph *sceneGraph = core_alloc(sizeof(vulkan_scene_graph));
  sceneGraph->data = NULL;
  sceneGraph->sceneTree = vulkan_scene_tree_create(sceneGraph);

  sceneGraph->root = NULL;
  vulkan_scene_graph_create_with_scene_data(sceneGraph, data);

  vulkan_scene_graph_set_dirty(sceneGraph, sceneGraph->root);
  vulkan_scene_tree_validate(sceneGraph->sceneTree);

  return sceneGraph;
}

void vulkan_scene_graph_destroy(vulkan_scene_graph *sceneGraph) {
  vulkan_scene_tree_destroy(sceneGraph->sceneTree);

  vulkan_scene_node *node, *tempNode;
  DL_FOREACH_SAFE(sceneGraph->root, node, tempNode) { vulkan_scene_node_destroy(node); }

  core_free(sceneGraph);
}

vulkan_scene_node *add_entity(vulkan_scene_graph *sceneGraph,
                              vulkan_scene_node *parentSceneGraphNode, vulkan_scene_node_type type,
                              void *entity) {
  /* add scene graph node */
  vulkan_scene_node *sceneGraphNode = NULL;

  bool uniqueSceneGraphNode = true;
  bool createNewSceneGraphNode = true;

  if (uniqueSceneGraphNode) {
    vulkan_scene_node *sceneNode = NULL;
    DL_FOREACH(sceneGraph->root, sceneNode) {
      if (sceneNode->entity == entity) {
        // Return already added node.
        assert(sceneNode->type == type);
        sceneGraphNode = sceneNode;
        createNewSceneGraphNode = false;
        break;
      }
    }
  }

  if (createNewSceneGraphNode) {
    sceneGraphNode = vulkan_scene_node_create(type, entity, false);
    DL_APPEND(sceneGraph->root, sceneGraphNode);
    sceneGraphNode->sceneGraph = sceneGraph;
  }

  if (parentSceneGraphNode != NULL) {
    assert(sceneGraphNode != parentSceneGraphNode);
    utarray_push_back(sceneGraphNode->parentNodes, &parentSceneGraphNode);
    if (sceneGraphNode->type == vulkan_scene_node_type_object) {
      utarray_push_back(parentSceneGraphNode->childObjectNodes, &sceneGraphNode);
    } else if (sceneGraphNode->type == vulkan_scene_node_type_mesh) {
      parentSceneGraphNode->meshNode = sceneGraphNode;
    } else if (sceneGraphNode->type == vulkan_scene_node_type_primitive) {
      utarray_push_back(parentSceneGraphNode->primitiveNodes, &sceneGraphNode);
    } else {
      verify(0);
    }
  }

  /* add scene tree node */

  if (type == vulkan_scene_node_type_root) {
    vulkan_scene_tree *sceneTree = sceneGraph->sceneTree;
    vulkan_scene_node *sceneTreeNode = vulkan_scene_node_create(type, entity, true);
    DL_APPEND(sceneTree->nodes, sceneTreeNode);
    utarray_push_back(sceneGraphNode->observers, &sceneTreeNode);
  } else {
    assert(utarray_len(parentSceneGraphNode->observers) >= 1);
    vulkan_scene_node *parentSceneTreeNode = NULL;

    vulkan_scene_node **observerNodeIt = NULL;
    while ((observerNodeIt = utarray_next(parentSceneGraphNode->observers, observerNodeIt))) {
      parentSceneTreeNode = *observerNodeIt;
      assert(parentSceneTreeNode != NULL);

      vulkan_scene_tree *sceneTree = sceneGraph->sceneTree;
      vulkan_scene_node *sceneTreeNode = vulkan_scene_node_create(type, entity, true);
      DL_APPEND(sceneTree->nodes, sceneTreeNode);

      utarray_push_back(sceneGraphNode->observers, &sceneTreeNode);
      utarray_push_back(sceneTreeNode->parentNodes, &parentSceneTreeNode);

      if (sceneTreeNode->type == vulkan_scene_node_type_object) {
        utarray_push_back(parentSceneTreeNode->childObjectNodes, &sceneTreeNode);
      } else if (sceneTreeNode->type == vulkan_scene_node_type_mesh) {
        parentSceneTreeNode->meshNode = sceneTreeNode;
      } else if (sceneTreeNode->type == vulkan_scene_node_type_primitive) {
        utarray_push_back(parentSceneTreeNode->primitiveNodes, &sceneTreeNode);
      } else {
        verify(0);
      }
    }
  }

  if (type == vulkan_scene_node_type_object) {
    vulkan_data_object *object = entity;

    if (object->mesh) {
      add_entity(sceneGraph, sceneGraphNode, vulkan_scene_node_type_mesh, object->mesh);
    }

    vulkan_data_object **childObjectIt = NULL;
    while ((childObjectIt = utarray_next(object->children, childObjectIt))) {
      add_entity(sceneGraph, sceneGraphNode, vulkan_scene_node_type_object, *childObjectIt);
    }
  } else if (type == vulkan_scene_node_type_mesh) {
    vulkan_data_mesh *mesh = entity;

    vulkan_data_primitive **primitiveIt = NULL;
    while ((primitiveIt = utarray_next(mesh->primitives, primitiveIt))) {
      add_entity(sceneGraph, sceneGraphNode, vulkan_scene_node_type_primitive, *primitiveIt);
    }
  }

  assert(sceneGraphNode != NULL);
  return sceneGraphNode;
}

void remove_entity(vulkan_scene_graph *sceneGraph, vulkan_scene_node *sceneGraphNode) {
  // FIXME: Simplify scene graph logic further - too many hoops to jump through right now.

  assert(sceneGraphNode->type == vulkan_scene_node_type_object);
  vulkan_scene_node_remove_util(sceneGraphNode, sceneGraph->root);

  // remove observers and their successors
  vulkan_scene_node **observerNodeIt = NULL;
  while ((observerNodeIt = utarray_next(sceneGraphNode->observers, observerNodeIt))) {
    vulkan_scene_node *sceneTreeNode = *observerNodeIt;
    vulkan_scene_node_remove_util(sceneTreeNode, sceneGraph->sceneTree->nodes);
    vulkan_scene_node_destroy(sceneTreeNode);
  }

  // free
  vulkan_scene_node_destroy(sceneGraphNode);
}

vulkan_scene_node *vulkan_scene_graph_add_object(vulkan_scene_graph *sceneGraph,
                                                 vulkan_scene_node *sceneNode,
                                                 vulkan_data_object *successorObject) {
  vulkan_scene_node *childNode =
      add_entity(sceneGraph, sceneNode, vulkan_scene_node_type_object, successorObject);

  vulkan_scene_node_set_dirty(childNode);

  return childNode;
}

void vulkan_scene_graph_remove_object(vulkan_scene_graph *sceneGraph,
                                      vulkan_scene_node *sceneNode) {
  remove_entity(sceneGraph, sceneNode);
}

void vulkan_scene_graph_set_dirty(vulkan_scene_graph *sceneGraph, vulkan_scene_node *sceneNode) {
  vulkan_scene_node **observerNodeIt = NULL;
  while ((observerNodeIt = utarray_next(sceneNode->observers, observerNodeIt))) {
    vulkan_scene_tree_set_dirty(sceneGraph->sceneTree, *observerNodeIt);
  }
}

void vulkan_scene_graph_create_with_scene_data(vulkan_scene_graph *sceneGraph,
                                               vulkan_data_scene *sceneData) {
  sceneGraph->data = sceneData;
  assert(sceneGraph->root == NULL);

  add_entity(sceneGraph, NULL, vulkan_scene_node_type_root, NULL);
  sceneGraph->sceneTree->root = *(vulkan_scene_node **)utarray_front(sceneGraph->root->observers);

  vulkan_data_object **rootObjectIt = NULL;
  while ((rootObjectIt = utarray_next(sceneGraph->data->rootObjects, rootObjectIt))) {
    vulkan_scene_graph_add_object(sceneGraph, sceneGraph->root, *rootObjectIt);
  }
  assert(sceneGraph->root);
}

void vulkan_scene_graph_debug_print(vulkan_scene_graph *sceneGraph) {
  log_raw(stdout, "digraph scene_graph { ");
  vulkan_scene_node_debug_print(sceneGraph->root);
  log_raw(stdout, " }\n");
}
