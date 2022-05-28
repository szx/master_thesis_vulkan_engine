#include "render_graph.h"

vulkan_render_graph *vulkan_render_graph_create(
    vulkan_swap_chain *vks, vulkan_renderer_cache *rendererCache, data_config *config,
    vulkan_unified_uniform_buffer_update_func updateGlobalUniformBufferFunc) {

  assert(dl_count(rendererCache->primitiveElements) > 0);

  vulkan_render_graph *renderGraph = core_alloc(sizeof(vulkan_render_graph));

  renderGraph->vks = vks;
  renderGraph->vkd = renderGraph->vks->vkd;

  renderGraph->rendererCache = rendererCache;
  renderGraph->config = config;
  renderGraph->updateGlobalUniformBufferFunc = updateGlobalUniformBufferFunc;
  renderGraph->vertexStream = vulkan_vertex_stream_create(renderGraph->rendererCache->attributes);

  renderGraph->unifiedGeometryBuffer = vulkan_unified_geometry_buffer_create(renderGraph->vkd);
  renderGraph->textures = vulkan_textures_create(renderGraph->vkd);
  renderGraph->unifiedUniformBuffer = vulkan_unified_uniform_buffer_create(
      renderGraph->vkd, renderGraph->rendererCache->maxPrimitiveElementCount);
  renderGraph->descriptors = vulkan_descriptors_create(
      renderGraph->vkd, renderGraph->unifiedUniformBuffer, renderGraph->textures);
  renderGraph->sync = vulkan_sync_create(renderGraph->vkd);

  return renderGraph;
}

void vulkan_render_graph_destroy(vulkan_render_graph *renderGraph) {
  vulkan_sync_destroy(renderGraph->sync);

  vulkan_descriptors_destroy(renderGraph->descriptors);
  vulkan_unified_uniform_buffer_destroy(renderGraph->unifiedUniformBuffer);
  vulkan_textures_destroy(renderGraph->textures);
  vulkan_unified_geometry_buffer_destroy(renderGraph->unifiedGeometryBuffer);

  vulkan_vertex_stream_destroy(renderGraph->vertexStream);

  core_free(renderGraph);
}

void vulkan_render_graph_update(vulkan_render_graph *renderGraph,
                                void *updateGlobalUniformBufferFuncData) {
  vulkan_renderer_cache_update_geometry(renderGraph->rendererCache, renderGraph->vertexStream);
  vulkan_renderer_cache_update_textures(renderGraph->rendererCache, renderGraph->textures);
  vulkan_vertex_stream_update(renderGraph->vertexStream);

  vulkan_unified_geometry_buffer_update(renderGraph->unifiedGeometryBuffer,
                                        renderGraph->vertexStream);
  vulkan_textures_update(renderGraph->textures);
  vulkan_unified_uniform_buffer_update(renderGraph->unifiedUniformBuffer, renderGraph->sync,
                                       renderGraph->updateGlobalUniformBufferFunc,
                                       updateGlobalUniformBufferFuncData);
  vulkan_descriptors_update(renderGraph->descriptors);
}

void vulkan_render_graph_send_to_device(vulkan_render_graph *renderGraph) {
  vulkan_unified_geometry_buffer_send_to_device(renderGraph->unifiedGeometryBuffer);
  vulkan_textures_send_to_device(renderGraph->textures);
  vulkan_unified_uniform_buffer_send_to_device(renderGraph->unifiedUniformBuffer);
  vulkan_descriptors_send_to_device(renderGraph->descriptors);

  assert(renderGraph->unifiedGeometryBuffer->indexBuffer->resident);
  assert(renderGraph->unifiedGeometryBuffer->vertexBuffer->resident);
  assert(renderGraph->unifiedUniformBuffer->buffer->resident);
}

void vulkan_render_graph_debug_print(vulkan_render_graph *renderGraph) {
  log_debug("render graph:\n");
  vulkan_unified_geometry_buffer_debug_print(renderGraph->unifiedGeometryBuffer);
  vulkan_unified_uniform_buffer_debug_print(renderGraph->unifiedUniformBuffer);
  vulkan_textures_debug_print(renderGraph->textures, 2);
  vulkan_descriptors_debug_print(renderGraph->descriptors, 2);
  vulkan_sync_debug_print(renderGraph->sync, 2);
}
