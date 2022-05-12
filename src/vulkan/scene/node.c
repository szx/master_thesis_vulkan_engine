#include "node.h"
#include "graph.h"

/* scene graph node */
vulkan_scene_graph_node *vulkan_scene_graph_node_create(vulkan_scene_graph *sceneGraph,
                                                        vulkan_asset_object *object,
                                                        vulkan_asset_primitive *primitive) {
  vulkan_scene_graph_node *sceneGraphNode = core_alloc(sizeof(vulkan_scene_graph_node));

  sceneGraphNode->sceneGraph = sceneGraph;
  sceneGraphNode->object = object;
  sceneGraphNode->primitive = primitive;

  utarray_alloc(sceneGraphNode->childNodes, sizeof(vulkan_scene_graph_node *));

  sceneGraphNode->prev = NULL;
  sceneGraphNode->next = NULL;

  utarray_alloc(sceneGraphNode->observers, sizeof(vulkan_scene_tree_node *));

  return sceneGraphNode;
}

void vulkan_scene_graph_node_destroy(vulkan_scene_graph_node *sceneGraphNode) {
  utarray_free(sceneGraphNode->observers);
  utarray_free(sceneGraphNode->childNodes);
  core_free(sceneGraphNode);
}

void vulkan_scene_graph_node_add_observer(vulkan_scene_graph_node *sceneGraphNode,
                                          vulkan_scene_tree_node *sceneTreeNode) {
  utarray_push_back(sceneGraphNode->observers, &sceneTreeNode);
}

void vulkan_scene_graph_node_add_child(vulkan_scene_graph_node *sceneGraphNode,
                                       vulkan_scene_graph_node *childNode) {
  utarray_push_back(sceneGraphNode->childNodes, &childNode);
}

void debug_log_graph_node(vulkan_scene_graph_node *sceneGraphNode) {
  log_raw(stdout, "\"%p\\n", sceneGraphNode);
  if (sceneGraphNode->object != NULL) {
    log_raw(stdout, "object: %p\\n", sceneGraphNode->object);
    if (sceneGraphNode->object->camera != NULL) {
      log_raw(stdout, "camera: %p\\n", sceneGraphNode->object->camera);
    }
  }
  if (sceneGraphNode->primitive != NULL) {
    log_raw(stdout, "vertex count: %d\\n", sceneGraphNode->primitive->vertexCount);
  }
  log_raw(stdout, "childNodes: %zu\\n", utarray_len(sceneGraphNode->childNodes));
  log_raw(stdout, "observers: %zu\\n", utarray_len(sceneGraphNode->observers));
  log_raw(stdout, "\"");
}

void vulkan_scene_graph_node_debug_print(vulkan_scene_graph_node *sceneGraphNode) {
  debug_log_graph_node(sceneGraphNode);
  log_raw(stdout, "; ");

  utarray_foreach_elem_deref (vulkan_scene_graph_node *, childNode, sceneGraphNode->childNodes) {
    debug_log_graph_node(sceneGraphNode);
    log_raw(stdout, " -> ");
    debug_log_graph_node(childNode);
    log_raw(stdout, "; ");
    vulkan_scene_graph_node_debug_print(childNode);
  }
}

/* scene tree node */

void vulkan_scene_tree_node_accumulated_init(vulkan_scene_tree_node_accumulated *accumulated) {
  accumulated->distanceFromRoot = 0;
  accumulated->visible = true;
  glm_mat4_identity(accumulated->transform);

  accumulated->primitive = NULL;

  accumulated->aabb = vulkan_aabb_default();
  accumulated->camera = NULL;
}

void vulkan_scene_tree_node_accumulated_deinit(vulkan_scene_tree_node_accumulated *accumulated) {}

void vulkan_scene_tree_node_accumulated_debug_print(
    vulkan_scene_tree_node_accumulated *accumulated) {
  log_raw(stdout, "\"accumulated\\n%p\\n", accumulated);
  log_raw(stdout, "distanceFromRoot: %d\\n", accumulated->distanceFromRoot);
  log_raw(stdout, "visible: %d\\n", accumulated->visible);
  log_raw(stdout, "transform: \\n" MAT4_FORMAT_STRING("\\n") "\\n",
          MAT4_FORMAT_ARGS(accumulated->transform));
  log_raw(stdout, "primitive: %p\\n", accumulated->primitive);
  log_raw(stdout, "aabb: \\n" VEC4_FORMAT_STRING() "\\n" VEC4_FORMAT_STRING() "\\n",
          VEC4_FORMAT_ARGS(accumulated->aabb.min), VEC4_FORMAT_ARGS(accumulated->aabb.max));
  log_raw(stdout, "camera: %p\\n", accumulated->camera);
  log_raw(stdout, "\"; ");
}

vulkan_scene_tree_node *vulkan_scene_tree_node_create(vulkan_scene_tree *sceneTree,
                                                      vulkan_scene_graph_node *sceneGraphNode) {
  vulkan_scene_tree_node *sceneTreeNode = core_alloc(sizeof(vulkan_scene_tree_node));

  sceneTreeNode->sceneTree = sceneTree;
  sceneTreeNode->object = sceneGraphNode->object;
  sceneTreeNode->primitive = sceneGraphNode->primitive;

  sceneTreeNode->parentNode = NULL;
  utarray_alloc(sceneTreeNode->childNodes, sizeof(vulkan_scene_tree_node *));

  sceneTreeNode->prev = NULL;
  sceneTreeNode->next = NULL;

  vulkan_scene_tree_node_accumulated_init(&sceneTreeNode->accumulated);

  vulkan_scene_tree_node_reset_accumulated(sceneTreeNode);
  sceneTreeNode->dirty = false;

  return sceneTreeNode;
}

void vulkan_scene_tree_node_destroy(vulkan_scene_tree_node *sceneTreeNode) {
  vulkan_scene_tree_node_accumulated_deinit(&sceneTreeNode->accumulated);
  utarray_free(sceneTreeNode->childNodes);
  core_free(sceneTreeNode);
}

void vulkan_scene_tree_node_add_parent(vulkan_scene_tree_node *sceneTreeNode,
                                       vulkan_scene_tree_node *parentNode) {
  assert(sceneTreeNode->parentNode == NULL);
  sceneTreeNode->parentNode = parentNode;
}

void vulkan_scene_tree_node_add_child(vulkan_scene_tree_node *sceneTreeNode,
                                      vulkan_scene_tree_node *childNode) {
  utarray_push_back(sceneTreeNode->childNodes, &childNode);
}

void vulkan_scene_tree_node_reset_accumulated(vulkan_scene_tree_node *sceneTreeNode) {
  assert(sceneTreeNode != NULL);

  if (sceneTreeNode->object != NULL) {
    glm_mat4_copy(sceneTreeNode->object->transform, sceneTreeNode->accumulated.transform);
  }

  if (sceneTreeNode->primitive != NULL) {
    assert(sceneTreeNode->accumulated.primitive == NULL); // primitive should be only on leaves
    sceneTreeNode->accumulated.primitive = sceneTreeNode->primitive;
    sceneTreeNode->accumulated.aabb =
        vulkan_asset_primitive_calculate_aabb(sceneTreeNode->accumulated.primitive);
  }

  if (sceneTreeNode->object != NULL && sceneTreeNode->object->camera != NULL) {
    sceneTreeNode->accumulated.camera = sceneTreeNode->object->camera;
  }
}

void vulkan_scene_tree_node_accumulate_from_parent(vulkan_scene_tree_node *sceneTreeNode) {
  vulkan_scene_tree_node *parentSceneTreeNode = sceneTreeNode->parentNode;

  vulkan_scene_tree_node_accumulated *accumulated = &sceneTreeNode->accumulated;
  vulkan_scene_tree_node_accumulated *parentAccumulated = &parentSceneTreeNode->accumulated;

  accumulated->distanceFromRoot = parentAccumulated->distanceFromRoot + 1;
  accumulated->visible = parentAccumulated->visible;
  glm_mat4_mul(parentAccumulated->transform, accumulated->transform, accumulated->transform);

  if (accumulated->primitive != NULL) {
    accumulated->aabb = vulkan_asset_primitive_calculate_aabb(accumulated->primitive);
    glm_mat4_mulv(accumulated->transform, accumulated->aabb.min, accumulated->aabb.min);
    glm_mat4_mulv(accumulated->transform, accumulated->aabb.max, accumulated->aabb.max);
  }

  assert(!(parentAccumulated->camera != NULL && accumulated->camera != NULL));
  if (parentAccumulated->camera != NULL && accumulated->camera == NULL) {
    // Do not accumulate camera and add only once.
    // accumulated->camera = parentAccumulated->camera;
  }
}
void debug_log_tree_node(vulkan_scene_tree_node *sceneTreeNode) {
  log_raw(stdout, "\"%p\\n", sceneTreeNode);
  if (sceneTreeNode->object != NULL) {
    log_raw(stdout, "object: %p\\n", sceneTreeNode->object);
    if (sceneTreeNode->object->camera != NULL) {
      log_raw(stdout, "camera: %p\\n", sceneTreeNode->object->camera);
    }
  }
  if (sceneTreeNode->primitive != NULL) {
    log_raw(stdout, "vertex count: %d\\n", sceneTreeNode->primitive->vertexCount);
  }
  log_raw(stdout, "childNodes: %zu\\n", utarray_len(sceneTreeNode->childNodes));
  log_raw(stdout, "\"");
}

void vulkan_scene_tree_node_debug_print(vulkan_scene_tree_node *sceneTreeNode) {
  debug_log_tree_node(sceneTreeNode);
  log_raw(stdout, "; ");

  log_raw(stdout, " { rank=same; ");
  debug_log_tree_node(sceneTreeNode);
  log_raw(stdout, " -> ");
  vulkan_scene_tree_node_accumulated_debug_print(&sceneTreeNode->accumulated);
  log_raw(stdout, " } ");

  utarray_foreach_elem_deref (vulkan_scene_tree_node *, childNode, sceneTreeNode->childNodes) {
    debug_log_tree_node(sceneTreeNode);
    log_raw(stdout, " -> ");
    debug_log_tree_node(childNode);
    log_raw(stdout, "; ");
    vulkan_scene_tree_node_debug_print(childNode);
  }
}
