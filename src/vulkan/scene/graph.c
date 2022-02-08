#include "graph.h"

vulkan_scene_graph *vulkan_scene_graph_create(vulkan_data_scene *data) {
  vulkan_scene_graph *sceneGraph = core_alloc(sizeof(vulkan_scene_graph));
  vulkan_scene_graph_create_with_scene_data(sceneGraph, data);
  return sceneGraph;
}

void vulkan_scene_graph_destroy(vulkan_scene_graph *sceneGraph) {
  vulkan_data_scene_destroy(sceneGraph->data);
  core_free(sceneGraph);
}

void vulkan_scene_graph_create_with_scene_data(vulkan_scene_graph *sceneGraph,
                                               vulkan_data_scene *sceneData) {
  assert(sceneGraph->root == NULL);
  sceneGraph->data = sceneData;
  // GIRO root sceneGraph->data->object
}

void vulkan_scene_graph_debug_print(vulkan_scene_graph *sceneGraph) {
  log_debug("SCENE GRAPH:\n");
  // TODO: Graphviz output.
  vulkan_data_scene_debug_print(sceneGraph->data);
}
