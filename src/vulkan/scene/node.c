#include "node.h"
#include "graph.h"

vulkan_scene_node *vulkan_scene_node_create(vulkan_scene_node_type type, void *entity,
                                            bool createCache) {
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

  sceneNode->sceneGraph = NULL;
  sceneNode->sceneTree = NULL;

  if (createCache) {
    sceneNode->cache = vulkan_scene_cache_create(sceneNode);
  } else {
    sceneNode->cache = NULL;
  }
  sceneNode->dirty = false;

  utarray_alloc(sceneNode->parentNodes, sizeof(vulkan_scene_node *));
  utarray_alloc(sceneNode->childObjectNodes, sizeof(vulkan_scene_node *));
  sceneNode->meshNode = NULL;
  utarray_alloc(sceneNode->primitiveNodes, sizeof(vulkan_scene_node *));
  utarray_alloc(sceneNode->observers, sizeof(vulkan_scene_node *));

  sceneNode->prev = NULL;
  sceneNode->next = NULL;
  return sceneNode;
}

void vulkan_scene_node_destroy(vulkan_scene_node *sceneNode) {
  if (sceneNode->cache) {
    vulkan_scene_cache_destroy(sceneNode->cache);
  }

  // NOTE: Successor and observer nodes are freed by scene graph and scene tree.
  utarray_free(sceneNode->parentNodes);
  utarray_free(sceneNode->childObjectNodes);
  utarray_free(sceneNode->primitiveNodes);
  utarray_free(sceneNode->observers);
  core_free(sceneNode);
}

void vulkan_scene_node_set_dirty(vulkan_scene_node *sceneNode) {
  assert(sceneNode);
  vulkan_scene_graph_set_dirty(sceneNode->sceneGraph, sceneNode);
}

void debug_log_node(vulkan_scene_node *sceneNode) {
  log_raw(stdout, "\"%p\\n%s\\nobservers: %zu\\n", sceneNode,
          vulkan_scene_node_type_debug_str(sceneNode->type), utarray_len(sceneNode->observers));
  if (sceneNode->type == vulkan_scene_node_type_object) {
    log_raw(stdout, "childObjectNodes: %zu\\n", utarray_len(sceneNode->childObjectNodes));
    log_raw(stdout, "meshNode: %p\\n", sceneNode->meshNode);
  } else if (sceneNode->type == vulkan_scene_node_type_mesh) {
    log_raw(stdout, "primitiveNodes: %zu\\n", utarray_len(sceneNode->primitiveNodes));
  } else if (sceneNode->type == vulkan_scene_node_type_primitive) {
    log_raw(stdout, "vertex count: %d\\n", sceneNode->primitive->vertexCount);
  }
  if (sceneNode->cache) {
    log_raw(stdout, "dirty: %d\\n", sceneNode->dirty);
  }
  log_raw(stdout, "\"", sceneNode->dirty);
}

void vulkan_scene_node_remove_util(vulkan_scene_node *sceneNode, vulkan_scene_node *nodes) {
  assert(sceneNode->type != vulkan_scene_node_type_root);

  // remove from list of nodes
  bool found = false;
  vulkan_scene_node *node, *tempNode;
  DL_FOREACH_SAFE(nodes, node, tempNode) {
    if (sceneNode == node) {
      found = true;
      DL_DELETE(nodes, sceneNode);
      break;
    }
  }
  if (!found) {
    log_error("deleting node not in nodes");
    return;
  }

  // remove from parents
  vulkan_scene_node **parentNodeIt = NULL;
  while ((parentNodeIt = utarray_next(sceneNode->parentNodes, parentNodeIt))) {
    vulkan_scene_node *parentSceneNode = *parentNodeIt;
    if (sceneNode->type == vulkan_scene_node_type_mesh) {
      assert(parentSceneNode->type == vulkan_scene_node_type_object);
      parentSceneNode->meshNode = NULL;
    } else {
      size_t idx = 0;
      vulkan_scene_node **successorIt = NULL;
      UT_array *successors = NULL;
      if (sceneNode->type == vulkan_scene_node_type_primitive) {
        successors = parentSceneNode->primitiveNodes;
      } else if (sceneNode->type == vulkan_scene_node_type_object) {
        successors = parentSceneNode->childObjectNodes;
      }
      while ((successorIt = utarray_next(successors, successorIt))) {
        vulkan_scene_node *successor = *successorIt;
        if (successor == sceneNode) {
          break;
        }
        idx++;
      }
      assert(idx < utarray_len(successors));
      utarray_erase(successors, idx, 1);
    }
  }
}

void vulkan_scene_node_debug_print(vulkan_scene_node *sceneNode) {
  debug_log_node(sceneNode);
  log_raw(stdout, "; ");

  if (sceneNode->cache) {
    log_raw(stdout, " { rank=same; ");
    debug_log_node(sceneNode);
    log_raw(stdout, " -> ");
    vulkan_scene_cache_debug_print(sceneNode->cache);
    log_raw(stdout, " } ");
  }

  vulkan_scene_node **successorIt = NULL;
  while ((successorIt = utarray_next(sceneNode->childObjectNodes, successorIt))) {
    vulkan_scene_node *successor = *successorIt;
    debug_log_node(sceneNode);
    log_raw(stdout, " -> ");
    debug_log_node(successor);
    log_raw(stdout, "; ");
    vulkan_scene_node_debug_print(successor);
  }
  if (sceneNode->meshNode != NULL) {
    debug_log_node(sceneNode);
    log_raw(stdout, " -> ");
    debug_log_node(sceneNode->meshNode);
    log_raw(stdout, "; ");
    vulkan_scene_node_debug_print(sceneNode->meshNode);
  }
  successorIt = NULL;
  while ((successorIt = utarray_next(sceneNode->primitiveNodes, successorIt))) {
    vulkan_scene_node *successor = *successorIt;
    debug_log_node(sceneNode);
    log_raw(stdout, " -> ");
    debug_log_node(successor);
    log_raw(stdout, "; ");
    vulkan_scene_node_debug_print(successor);
  }
}
