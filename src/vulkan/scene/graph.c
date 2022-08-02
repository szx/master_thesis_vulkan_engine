#include "graph.h"
#include "../renderers/renderer_cache.h"
#include "tree.h"

scene_graph *scene_graph_create(scene_data *data, renderer_cache *rendererCache) {
  scene_graph *sceneGraph = core_alloc(sizeof(scene_graph));
  sceneGraph->data = NULL;
  sceneGraph->sceneTree = scene_tree_create(sceneGraph, rendererCache);
  sceneGraph->root = NULL;
  sceneGraph->nodes = NULL;

  scene_graph_create_with_scene_data(sceneGraph, data);

  return sceneGraph;
}

void scene_graph_destroy(scene_graph *sceneGraph) {
  if (sceneGraph->root->object != NULL) {
    asset_object_deinit(sceneGraph->root->object);
    core_free(sceneGraph->root->object);
  }

  scene_tree_destroy(sceneGraph->sceneTree);

  dl_foreach_elem(scene_graph_node *, node, sceneGraph->root) { scene_graph_node_destroy(node); }

  core_free(sceneGraph);
}

scene_graph_node *add_entity(scene_graph *sceneGraph, scene_graph_node *parentSceneGraphNode,
                             asset_object *object, asset_primitive *primitive,
                             bool createSceneGraphNode) {
  /* add scene graph node */
  scene_graph_node *sceneGraphNode = NULL;

  bool uniqueSceneGraphNode = true;
  bool existsSceneGraphNode = false;

  if (uniqueSceneGraphNode) {
    dl_foreach_elem(scene_graph_node *, sceneNode, sceneGraph->nodes) {
      if (sceneNode->object == object && sceneNode->primitive == primitive) {
        // Return already added node.
        sceneGraphNode = sceneNode;
        existsSceneGraphNode = true;
        break;
      }
    }
  }

  if (createSceneGraphNode && !existsSceneGraphNode) {
    sceneGraphNode = scene_graph_node_create(sceneGraph, object, primitive);
    DL_APPEND(sceneGraph->nodes, sceneGraphNode);
  }
  assert(sceneGraphNode != NULL);

  if (parentSceneGraphNode != NULL) {
    assert(sceneGraphNode != parentSceneGraphNode);
    if (createSceneGraphNode) {
      scene_graph_node_add_child(parentSceneGraphNode, sceneGraphNode);
    }
  }

  /* add child graph nodes */

  if (parentSceneGraphNode == NULL) {
    // root
    scene_tree_node *sceneTreeNode = scene_tree_node_create(sceneGraph->sceneTree, sceneGraphNode);
    DL_APPEND(sceneGraph->sceneTree->nodes, sceneTreeNode);
    scene_graph_node_add_observer(sceneGraphNode, sceneTreeNode);
  } else if (sceneGraphNode->object != NULL) {

    if (sceneGraphNode->object->mesh != NULL) {
      utarray_foreach_elem_deref (asset_primitive *, childPrimitive,
                                  sceneGraphNode->object->mesh->primitives) {
        scene_graph_node *childSceneGraphNode =
            add_entity(sceneGraph, sceneGraphNode, NULL, childPrimitive, !existsSceneGraphNode);
      }
    }
    utarray_foreach_elem_deref (asset_object *, childObject, object->children) {
      scene_graph_node *childSceneGraphNode =
          add_entity(sceneGraph, sceneGraphNode, childObject, NULL, !existsSceneGraphNode);
    }
  }

  assert(sceneGraphNode != NULL);
  return sceneGraphNode;
}

scene_graph_node *scene_graph_add_object(scene_graph *sceneGraph, scene_graph_node *sceneGraphNode,
                                         scene_tree_node *sceneTreeNode,
                                         asset_object *successorObject) {
  scene_graph_node *childSceneGraphNode =
      add_entity(sceneGraph, sceneGraphNode, successorObject, NULL, true);

  scene_tree_node *childSceneTreeNode = scene_tree_add_node(
      sceneGraph->sceneTree, childSceneGraphNode, sceneGraphNode, sceneTreeNode);

  return childSceneGraphNode;
}

void scene_graph_set_dirty(scene_graph *sceneGraph, scene_graph_node *sceneGraphNode) {
  utarray_foreach_elem_deref (scene_tree_node *, observerNode, sceneGraphNode->observers) {
    scene_tree_set_dirty(sceneGraph->sceneTree, observerNode);
  }
}

void scene_graph_create_with_scene_data(scene_graph *sceneGraph, scene_data *sceneData) {
  sceneGraph->data = sceneData;
  assert(sceneGraph->root == NULL);
  assert(sceneGraph->nodes == NULL);

  /* construct scene graph and scene tree */
  asset_object *rootObject = core_alloc(sizeof(asset_object));
  asset_object_init(rootObject, sceneGraph->data);
  glm_mat4_identity(rootObject->transform);
  // NOTE: Change right-handed model-space into left-handed world-space.
  rootObject->transform[2][2] = -1.0f;
  sceneGraph->root = add_entity(sceneGraph, NULL, rootObject, NULL, true);
  sceneGraph->sceneTree->root = *(scene_tree_node **)utarray_front(sceneGraph->root->observers);

  utarray_foreach_elem_deref (asset_object *, rootChild, sceneGraph->data->rootObjects) {
    scene_graph_add_object(sceneGraph, sceneGraph->root, sceneGraph->sceneTree->root, rootChild);
  }

  /* validate and accumulate */
  scene_graph_set_dirty(sceneGraph, sceneGraph->root);
  scene_tree_validate(sceneGraph->sceneTree);

  /* add additional state to renderer cache */
  // PERF: Instead of iterating over every scene tree node, iterate just by leaves and camera nodes.
  dl_foreach_elem(scene_tree_node *, sceneTreeNode, sceneGraph->sceneTree->nodes) {
    scene_tree_node_accumulated *accumulated = &sceneTreeNode->accumulated;
    if (accumulated->primitive != NULL) {
      log_debug("adding renderer cache primitive element");
      renderer_cache_primitive_element *primitiveElement = renderer_cache_primitive_element_create(
          renderer_cache_primitive_element_source_type_scene_tree, accumulated->visible,
          accumulated->transform, accumulated->primitive, accumulated->aabb);
      renderer_cache_add_primitive_element(sceneGraph->sceneTree->rendererCache, primitiveElement);
    }
    if (accumulated->camera != NULL) {
      log_debug("adding renderer cache camera element");
      renderer_cache_camera_element *cameraElement =
          renderer_cache_camera_element_create(accumulated->camera, accumulated->transform);
      renderer_cache_add_camera_element(sceneGraph->sceneTree->rendererCache, cameraElement);
    }
  }

  dl_foreach_elem(asset_direct_light *, directLight, sceneData->directLights) {
    if (directLight == scene_data_get_default_direct_light(sceneData)) {
      continue;
    }
    log_debug("adding renderer cache direct light element");
    renderer_cache_direct_light_element *directLightElement =
        renderer_cache_direct_light_element_create(directLight);
    renderer_cache_add_direct_light_element(sceneGraph->sceneTree->rendererCache,
                                            directLightElement);
  }

  log_debug("adding renderer cache skybox element");
  renderer_cache_skybox_element *skyboxElement =
      renderer_cache_skybox_element_create(sceneData->skybox);
  renderer_cache_add_skybox(sceneGraph->sceneTree->rendererCache, skyboxElement);

  log_debug("adding renderer cache font element");
  renderer_cache_font_element *fontElement = renderer_cache_font_element_create(sceneData->font);
  renderer_cache_add_font(sceneGraph->sceneTree->rendererCache, fontElement);

  assert(sceneGraph->root);
}

void scene_graph_debug_print(scene_graph *sceneGraph) {
  log_raw(stdout, "digraph scene_graph { ");
  scene_graph_node_debug_print(sceneGraph->root);
  log_raw(stdout, " }\n");
  scene_tree_debug_print(sceneGraph->sceneTree);
}
