#include "node.h"

vulkan_scene_node *vulkan_scene_node_create(vulkan_scene_node_type type, void *entity) {
  vulkan_scene_node *sceneNode = core_alloc(sizeof(vulkan_scene_node));

  sceneNode->type = type;
  sceneNode->entity = entity;
  if (sceneNode->type == vulkan_scene_node_type_object) {
    sceneNode->object = entity;
  } else if (sceneNode->type == vulkan_scene_node_type_mesh) {
    sceneNode->mesh = entity;
  } else if (sceneNode->type == vulkan_scene_node_type_primitive) {
    sceneNode->primitive = entity;
  } else if (sceneNode->type != vulkan_scene_node_type_root) {
    assert(0);
  }

  utarray_alloc(sceneNode->successors, sizeof(vulkan_scene_node *));

  utarray_alloc(sceneNode->observers, sizeof(vulkan_scene_node *));

  sceneNode->prev = NULL;
  sceneNode->next = NULL;
  return sceneNode;
}

void vulkan_scene_node_destroy(vulkan_scene_node *sceneNode) {
  // NOTE: Successor and observer nodes are freed by scene graph and scene tree.
  utarray_free(sceneNode->successors);
  utarray_free(sceneNode->observers);
  core_free(sceneNode);
}

void vulkan_scene_node_add_successor(vulkan_scene_node *sceneNode,
                                     vulkan_scene_node *successorNode) {
  utarray_push_back(sceneNode->successors, &successorNode);
}

void vulkan_scene_node_add_observer(vulkan_scene_node *sceneNode, vulkan_scene_node *observerNode) {
  utarray_push_back(sceneNode->observers, &observerNode);
}

void debug_log_node(vulkan_scene_node *sceneNode) {
  log_raw(stdout, "%s_%p_successor_%zu_observer_%zu",
          vulkan_scene_node_type_debug_str(sceneNode->type), sceneNode,
          utarray_len(sceneNode->successors), utarray_len(sceneNode->observers));
  if (sceneNode->type == vulkan_scene_node_type_primitive) {
    log_raw(stdout, "_vertexCount_%d", sceneNode->primitive->vertexCount);
  }
}

void vulkan_scene_node_debug_print(vulkan_scene_node *sceneNode) {
  debug_log_node(sceneNode);
  log_raw(stdout, "; ");

  vulkan_scene_node **successorIt = NULL;
  while ((successorIt = utarray_next(sceneNode->successors, successorIt))) {
    vulkan_scene_node *successor = *successorIt;
    debug_log_node(sceneNode);
    log_raw(stdout, " -> ");
    debug_log_node(successor);
    log_raw(stdout, "; ");

    vulkan_scene_node_debug_print(successor);
  }
}
