#include "node.h"
#include "graph.h"

vulkan_scene_node *create_common(vulkan_scene_node_entity_type type, void *entity) {
  vulkan_scene_node *sceneNode = core_alloc(sizeof(vulkan_scene_node));

  sceneNode->type = type;
  sceneNode->entity = entity;
  if (sceneNode->type == vulkan_scene_node_entity_type_object) {
    sceneNode->object = entity;
  } else if (sceneNode->type == vulkan_scene_node_entity_type_primitive) {
    sceneNode->primitive = entity;
  } else if (sceneNode->type != vulkan_scene_node_entity_type_root) {
    assert(0);
  }

  utarray_alloc(sceneNode->parentNodes, sizeof(vulkan_scene_node *));
  utarray_alloc(sceneNode->childObjectNodes, sizeof(vulkan_scene_node *));
  utarray_alloc(sceneNode->primitiveNodes, sizeof(vulkan_scene_node *));

  sceneNode->prev = NULL;
  sceneNode->next = NULL;

  return sceneNode;
}

vulkan_scene_node *vulkan_scene_node_create_for_scene_graph(vulkan_scene_node_entity_type type,
                                                            void *entity,
                                                            vulkan_scene_graph *sceneGraph) {
  vulkan_scene_node *sceneNode = create_common(type, entity);

  sceneNode->containerType = vulkan_scene_node_container_type_scene_graph;
  sceneNode->sceneGraph = sceneGraph;
  utarray_alloc(sceneNode->observers, sizeof(vulkan_scene_node *));

  return sceneNode;
}

vulkan_scene_node *vulkan_scene_node_create_for_scene_tree(vulkan_scene_node_entity_type type,
                                                           void *entity,
                                                           vulkan_scene_tree *sceneTree) {
  vulkan_scene_node *sceneNode = create_common(type, entity);

  sceneNode->containerType = vulkan_scene_node_container_type_scene_tree;
  sceneNode->sceneTree = sceneTree;
  sceneNode->cache = vulkan_scene_cache_create(sceneNode);
  sceneNode->dirty = false;

  return sceneNode;
}

void vulkan_scene_node_destroy(vulkan_scene_node *sceneNode) {
  if (sceneNode->containerType == vulkan_scene_node_container_type_scene_graph) {
    utarray_free(sceneNode->observers);
  } else if (sceneNode->containerType == vulkan_scene_node_container_type_scene_tree) {
    vulkan_scene_cache_destroy(sceneNode->cache);
  } else {
    assert(0);
  }

  // NOTE: Successor and observer nodes are freed by scene graph and scene tree.
  utarray_free(sceneNode->parentNodes);
  utarray_free(sceneNode->childObjectNodes);
  utarray_free(sceneNode->primitiveNodes);
  core_free(sceneNode);
}

void vulkan_scene_node_set_dirty(vulkan_scene_node *sceneNode) {
  assert(sceneNode);
  vulkan_scene_graph_set_dirty(sceneNode->sceneGraph, sceneNode);
}

void debug_log_node(vulkan_scene_node *sceneNode) {
  log_raw(stdout, "\"%p\\n%s\\n", sceneNode,
          vulkan_scene_node_entity_type_debug_str(sceneNode->type));
  if (sceneNode->type == vulkan_scene_node_entity_type_object) {
    log_raw(stdout, "childObjectNodes: %zu\\n", utarray_len(sceneNode->childObjectNodes));
    log_raw(stdout, "primitiveNodes: %zu\\n", utarray_len(sceneNode->primitiveNodes));
  } else if (sceneNode->type == vulkan_scene_node_entity_type_primitive) {
    log_raw(stdout, "vertex count: %d\\n", sceneNode->primitive->vertexCount);
    log_raw(stdout, "geometry hash: %d\\n", sceneNode->primitive->geometryHash);
  }
  if (sceneNode->containerType == vulkan_scene_node_container_type_scene_graph) {
    log_raw(stdout, "observers: %zu\\n", utarray_len(sceneNode->observers));
  } else if (sceneNode->containerType == vulkan_scene_node_container_type_scene_tree) {
    log_raw(stdout, "dirty: %d\\n", sceneNode->dirty);
  } else {
    assert(0);
  }
  log_raw(stdout, "\"");
}

void vulkan_scene_node_remove_util(vulkan_scene_node *sceneNode, vulkan_scene_node *nodes) {
  assert(sceneNode->type != vulkan_scene_node_entity_type_root);

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
    size_t idx = 0;
    UT_array *successors = NULL;
    if (sceneNode->type == vulkan_scene_node_entity_type_primitive) {
      successors = parentSceneNode->primitiveNodes;
    } else if (sceneNode->type == vulkan_scene_node_entity_type_object) {
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

void vulkan_scene_node_debug_print(vulkan_scene_node *sceneNode) {
  debug_log_node(sceneNode);
  log_raw(stdout, "; ");

  if (sceneNode->containerType == vulkan_scene_node_container_type_scene_tree) {
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
  utarray_foreach_elem_deref (vulkan_scene_node *, primitiveNode, sceneNode->primitiveNodes) {
    debug_log_node(sceneNode);
    log_raw(stdout, " -> ");
    debug_log_node(primitiveNode);
    log_raw(stdout, "; ");
    vulkan_scene_node_debug_print(primitiveNode);
  }
}
