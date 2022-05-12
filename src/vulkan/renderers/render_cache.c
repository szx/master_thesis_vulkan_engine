#include "render_cache.h"

#include "../assets/mesh.h"
#include "../assets/primitive.h"
#include "../objects/textures.h"
#include "../scene/node.h"

vulkan_render_cache *vulkan_render_cache_create() {
  vulkan_render_cache *renderCache = core_alloc(sizeof(vulkan_render_cache));

  renderCache->prev = NULL;
  renderCache->next = NULL;

  vulkan_render_cache_reset(renderCache);
  renderCache->vertexStreamElement = (vulkan_vertex_stream_element){0};

  renderCache->instanceId = 0;

  renderCache->materialElement = 0;

  return renderCache;
}

void vulkan_render_cache_destroy(vulkan_render_cache *renderCache) { core_free(renderCache); }

void vulkan_render_cache_reset(vulkan_render_cache *renderCache) {
  renderCache->distanceFromRoot = 0;

  renderCache->visible = true;

  glm_mat4_identity(renderCache->transform);
  renderCache->mesh = NULL;
  vulkan_asset_camera_init(&renderCache->camera, NULL);

  renderCache->primitive = NULL;

  renderCache->aabb = vulkan_aabb_default();
}

void debug_log_cache(vulkan_render_cache *renderCache) {
  log_raw(stdout, "\"cache\\n%p\\n", renderCache);
  log_raw(stdout, "distanceFromRoot: %d\\n", renderCache->distanceFromRoot);
  log_raw(stdout, "visible: %d\\n", renderCache->visible);
  log_raw(stdout, "transform: \\n" MAT4_FORMAT_STRING("\\n") "\\n",
          MAT4_FORMAT_ARGS(renderCache->transform));
  log_raw(stdout, "mesh: %p\\n", renderCache->mesh);
  log_raw(stdout, "cameraFovY %f\\n", renderCache->camera.fovY);
  log_raw(stdout, "primitive: %p\\n", renderCache->primitive);
  log_raw(stdout, "firstIndexOffset: %zu\\n", renderCache->vertexStreamElement.firstIndexOffset);
  log_raw(stdout, "firstVertexOffset: %zu\\n", renderCache->vertexStreamElement.firstVertexOffset);
  log_raw(stdout, "instanceId: %zu", renderCache->instanceId);
  if (renderCache->materialElement != NULL) {
    log_raw(stdout, "materialId: %zu", renderCache->materialElement->materialIdx);
  }
  log_raw(stdout, "\"");
}

void vulkan_render_cache_debug_print(vulkan_render_cache *renderCache) {
  debug_log_cache(renderCache);
  log_raw(stdout, "; ");
}
