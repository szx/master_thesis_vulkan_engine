#include "graph.h"
#include "tree.h"

vulkan_scene_graph *vulkan_scene_graph_create(vulkan_data_scene *data,
                                              vulkan_render_cache_list *renderCacheList) {
  vulkan_scene_graph *sceneGraph = core_alloc(sizeof(vulkan_scene_graph));
  sceneGraph->data = NULL;
  sceneGraph->sceneTree = vulkan_scene_tree_create(sceneGraph, renderCacheList);
  sceneGraph->root = NULL;
  sceneGraph->nodes = NULL;

  vulkan_scene_graph_create_with_scene_data(sceneGraph, data);

  vulkan_scene_graph_set_dirty(sceneGraph, sceneGraph->root);
  vulkan_scene_tree_validate(sceneGraph->sceneTree);

  vulkan_render_cache_list_add_camera_render_cache(sceneGraph->sceneTree->renderCacheList,
                                                   sceneGraph->sceneTree->root->renderCache);

  return sceneGraph;
}

void vulkan_scene_graph_destroy(vulkan_scene_graph *sceneGraph) {
  if (sceneGraph->root->object != NULL) {
    vulkan_data_object_deinit(sceneGraph->root->object);
    core_free(sceneGraph->root->object);
  }

  vulkan_scene_tree_destroy(sceneGraph->sceneTree);

  dl_foreach_elem(vulkan_scene_graph_node *, node, sceneGraph->root,
                  { vulkan_scene_graph_node_destroy(node); })

  core_free(sceneGraph);
}

vulkan_scene_graph_node *add_entity(vulkan_scene_graph *sceneGraph,
                                    vulkan_scene_graph_node *parentSceneGraphNode,
                                    vulkan_data_object *object, vulkan_data_primitive *primitive,
                                    bool createSceneGraphNode) {
  /* add scene graph node */
  vulkan_scene_graph_node *sceneGraphNode = NULL;

  bool uniqueSceneGraphNode = true;
  bool existsSceneGraphNode = false;

  if (uniqueSceneGraphNode) {
    dl_foreach_elem_new(vulkan_scene_graph_node *, sceneNode, sceneGraph->nodes) {
      if (sceneNode->object == object && sceneNode->primitive == primitive) {
        // Return already added node.
        sceneGraphNode = sceneNode;
        existsSceneGraphNode = true;
        break;
      }
    }
  }

  if (createSceneGraphNode && !existsSceneGraphNode) {
    sceneGraphNode = vulkan_scene_graph_node_create(sceneGraph, object, primitive);
    DL_APPEND(sceneGraph->nodes, sceneGraphNode);
  }
  assert(sceneGraphNode != NULL);

  if (parentSceneGraphNode != NULL) {
    assert(sceneGraphNode != parentSceneGraphNode);
    if (createSceneGraphNode) {
      vulkan_scene_graph_node_add_child(parentSceneGraphNode, sceneGraphNode);
    }
  }

  /* add child graph nodes */

  if (parentSceneGraphNode == NULL) {
    // root
    vulkan_scene_tree_node *sceneTreeNode =
        vulkan_scene_tree_node_create(sceneGraph->sceneTree, sceneGraphNode);
    DL_APPEND(sceneGraph->sceneTree->nodes, sceneTreeNode);
    vulkan_scene_graph_node_add_observer(sceneGraphNode, sceneTreeNode);
  } else if (sceneGraphNode->object != NULL) {

    if (sceneGraphNode->object->mesh != NULL) {
      utarray_foreach_elem_deref (vulkan_data_primitive *, childPrimitive,
                                  sceneGraphNode->object->mesh->primitives) {
        vulkan_scene_graph_node *childSceneGraphNode =
            add_entity(sceneGraph, sceneGraphNode, NULL, childPrimitive, !existsSceneGraphNode);
      }
    }
    utarray_foreach_elem_deref (vulkan_data_object *, childObject, object->children) {
      vulkan_scene_graph_node *childSceneGraphNode =
          add_entity(sceneGraph, sceneGraphNode, childObject, NULL, !existsSceneGraphNode);
    }
  }

  assert(sceneGraphNode != NULL);
  return sceneGraphNode;
}

vulkan_scene_graph_node *vulkan_scene_graph_add_object(vulkan_scene_graph *sceneGraph,
                                                       vulkan_scene_graph_node *sceneGraphNode,
                                                       vulkan_scene_tree_node *sceneTreeNode,
                                                       vulkan_data_object *successorObject) {
  vulkan_scene_graph_node *childSceneGraphNode =
      add_entity(sceneGraph, sceneGraphNode, successorObject, NULL, true);

  vulkan_scene_tree_node *childSceneTreeNode = vulkan_scene_tree_add_node(
      sceneGraph->sceneTree, childSceneGraphNode, sceneGraphNode, sceneTreeNode);

  return childSceneGraphNode;
}

void vulkan_scene_graph_set_dirty(vulkan_scene_graph *sceneGraph,
                                  vulkan_scene_graph_node *sceneGraphNode) {
  utarray_foreach_elem_deref (vulkan_scene_tree_node *, observerNode, sceneGraphNode->observers) {
    vulkan_scene_tree_set_dirty(sceneGraph->sceneTree, observerNode);
  }
}

void vulkan_scene_graph_create_with_scene_data(vulkan_scene_graph *sceneGraph,
                                               vulkan_data_scene *sceneData) {
  sceneGraph->data = sceneData;
  assert(sceneGraph->root == NULL);
  assert(sceneGraph->nodes == NULL);

  vulkan_data_object *rootObject = core_alloc(sizeof(vulkan_data_object));
  vulkan_data_object_init(rootObject, sceneGraph->data);
  glm_mat4_identity(rootObject->transform);
  rootObject->transform[2][2] = -1.0f; /// Change right-handed into left-handed.
  sceneGraph->root = add_entity(sceneGraph, NULL, rootObject, NULL, true);
  sceneGraph->sceneTree->root =
      *(vulkan_scene_tree_node **)utarray_front(sceneGraph->root->observers);

  utarray_foreach_elem_deref (vulkan_data_object *, rootChild, sceneGraph->data->rootObjects) {
    vulkan_scene_graph_add_object(sceneGraph, sceneGraph->root, sceneGraph->sceneTree->root,
                                  rootChild);
  }

  assert(sceneGraph->root);
}

void vulkan_scene_graph_debug_print(vulkan_scene_graph *sceneGraph) {
  log_raw(stdout, "digraph scene_graph { ");
  vulkan_scene_graph_node_debug_print(sceneGraph->root);
  log_raw(stdout, " }\n");
  vulkan_scene_tree_debug_print(sceneGraph->sceneTree);
}
