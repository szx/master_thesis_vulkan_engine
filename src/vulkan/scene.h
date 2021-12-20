/* Vulkan scene and glTF parsing */

#ifndef VULKAN_SCENE_H
#define VULKAN_SCENE_H

#include "../core/platform.h"

#include "device.h"
#include "functions.h"

typedef struct gltf_buffer_view {
  char *name;
  size_t offset;
  size_t size;
  size_t stride;
  vulkan_geometry_buffer *buffer;
} gltf_buffer_view;

void gltf_buffer_view_init(gltf_buffer_view *bufferView, char *name, size_t offset, size_t size,
                           size_t stride, vulkan_geometry_buffer *buffer);
void gltf_buffer_view_deinit(gltf_buffer_view *bufferView);

typedef struct gltf_accessor {
  char *name;
  size_t offset;
  size_t count;
  size_t stride;
  gltf_buffer_view *bufferView;
} gltf_accessor;

void gltf_accessor_init(gltf_accessor *accessor, char *name, size_t offset, size_t size,
                        size_t stride, gltf_buffer_view *bufferView);
void gltf_accessor_deinit(gltf_accessor *accessor);

typedef struct gltf_attribute {
  char *name;
  vulkan_attribute_type type;
  gltf_accessor *accessor;
} gltf_attribute;

void gltf_attribute_init(gltf_attribute *attribute, char *name, vulkan_attribute_type type,
                         gltf_accessor *accessor);
void gltf_attribute_deinit(gltf_attribute *attribute);
int gltf_attribute_compare(const void *s1, const void *s2);

/// Contains vertex stream for a part of the mesh.
typedef struct vulkan_mesh_primitive {
  VkPrimitiveTopology topology;
  uint32_t vertexCount;
  vulkan_attribute_type vertexTypes;
  uint32_t vertexStride;  /// Calculated using vertexTypes.
  UT_array *vertexStream; /// vulkan_vertex_stream_element

  uint32_t indexCount;
  vulkan_index_type indexType;
  uint32_t indexStride;  /// Calculated using indexType.
  UT_array *indexBuffer; /// uint32_t or uint16_t

  // HIRO: Construct scene geometry buffer.
} vulkan_mesh_primitive;

void vulkan_mesh_primitive_init(vulkan_mesh_primitive *primitive, VkPrimitiveTopology topology,
                                vulkan_attribute_type vertexTypes, vulkan_index_type indexType);
void vulkan_mesh_primitive_deinit(vulkan_mesh_primitive *primitive);

typedef struct vulkan_mesh {
  vulkan_mesh_primitive *primitives;
} vulkan_mesh;

void vulkan_mesh_init(vulkan_mesh *mesh, size_t primitiveCount);
void vulkan_mesh_deinit(vulkan_mesh *mesh);

typedef struct vulkan_node {
  vulkan_mesh mesh;
  mat4 modelMat;
} vulkan_node;

void vulkan_node_init(vulkan_node *scene, vulkan_mesh mesh);
void vulkan_node_deinit(vulkan_node *scene);

// Describes a scene.
// Does not create any Vulkan objects (see vulkan_render_context).
typedef struct vulkan_scene {
  vulkan_node *nodes;
  vulkan_geometry_buffer *geometryBuffer;
  // HIRO VK_EXT_vertex_input_dynamic_state is not supported by Intel.
  // HIRO VK_EXT_extended_dynamic_state allows VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT
  // HIRO do not interleave buffer, different binding for every attribute?
  // HIRO What about vertex attribute descriptions? They assume interleaved attributes.
  // HIRO do not expose gltf_buffer_view and gltf_accessor after interleaving.
  // HIRO roll out our own mesh data format
  gltf_buffer_view *bufferViews;
  gltf_accessor *accessors;
} vulkan_scene;

void vulkan_scene_init(vulkan_scene *self, size_t nodesCount, size_t bufferViewsCount,
                       size_t accessorsCount);
void vulkan_scene_deinit(vulkan_scene *self);
void vulkan_scene_debug_print(vulkan_scene *self);

void parse_gltf_file(vulkan_scene *scene, platform_path gltfPath);

#include "../codegen/scene.h"

#endif /* !VULKAN_SCENE_H */
