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
  utarray_foreach_elem_deref (vulkan_scene_node *, parentSceneNode, sceneNode->parentNodes) {
    if (sceneNode->type == vulkan_scene_node_type_mesh) {
      assert(parentSceneNode->type == vulkan_scene_node_type_object);
      parentSceneNode->meshNode = NULL;
    } else {
      size_t idx = 0;
      UT_array *successors = NULL;
      if (sceneNode->type == vulkan_scene_node_type_primitive) {
        successors = parentSceneNode->primitiveNodes;
      } else if (sceneNode->type == vulkan_scene_node_type_object) {
        successors = parentSceneNode->childObjectNodes;
      }
      utarray_foreach_elem_deref (vulkan_scene_node *, successor, successors) {
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

  utarray_foreach_elem_deref (vulkan_scene_node *, childObjectNode, sceneNode->childObjectNodes) {
    debug_log_node(sceneNode);
    log_raw(stdout, " -> ");
    debug_log_node(childObjectNode);
    log_raw(stdout, "; ");
    vulkan_scene_node_debug_print(childObjectNode);
  }
  if (sceneNode->meshNode != NULL) {
    debug_log_node(sceneNode);
    log_raw(stdout, " -> ");
    debug_log_node(sceneNode->meshNode);
    log_raw(stdout, "; ");
    vulkan_scene_node_debug_print(sceneNode->meshNode);
  }
  utarray_foreach_elem_deref (vulkan_scene_node *, primitiveNode, sceneNode->primitiveNodes) {
    debug_log_node(sceneNode);
    log_raw(stdout, " -> ");
    debug_log_node(primitiveNode);
    log_raw(stdout, "; ");
    vulkan_scene_node_debug_print(primitiveNode);
  }
}
