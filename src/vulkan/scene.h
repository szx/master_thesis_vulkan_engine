/* Vulkan scene and glTF parsing */

#ifndef VULKAN_SCENE_H
#define VULKAN_SCENE_H

#include "../core/platform.h"

#include "device.h"
#include "functions.h"

typedef struct vulkan_buffer_view {
  char *name;
  size_t offset;
  size_t size;
  size_t stride;
  vulkan_geometry_buffer *buffer;
} vulkan_buffer_view;

void vulkan_buffer_view_init(vulkan_buffer_view *bufferView, char *name, size_t offset, size_t size,
                             size_t stride, vulkan_geometry_buffer *buffer);
void vulkan_buffer_view_deinit(vulkan_buffer_view *bufferView);

typedef struct vulkan_accessor {
  char *name;
  size_t offset;
  size_t count;
  size_t stride;
  vulkan_buffer_view *bufferView;
} vulkan_accessor;

void vulkan_accessor_init(vulkan_accessor *accessor, char *name, size_t offset, size_t size,
                          size_t stride, vulkan_buffer_view *bufferView);
void vulkan_accessor_free(vulkan_accessor *accessor);

typedef enum vulkan_attribute_type {
  UnknownAttribute,
  PositionAttribute,
  NormalAttribute,
  TangentAttribute,
  TexCoordAttribute,
  ColorAttribute
} vulkan_attribute_type;

typedef struct vulkan_attribute {
  char *name;
  vulkan_attribute_type type;
  size_t index;
  vulkan_accessor *accessor;
} vulkan_attribute;

void vulkan_attribute_init(vulkan_attribute *attribute, char *name, vulkan_attribute_type type,
                           size_t index, vulkan_accessor *accessor);
void vulkan_attribute_deinit(vulkan_attribute *attribute);

typedef struct vulkan_mesh_primitive {
  VkPrimitiveTopology topology;
  vulkan_attribute *attributes;
  vulkan_accessor *indices;
} vulkan_mesh_primitive;

void vulkan_mesh_primitive_init(vulkan_mesh_primitive *primitive, VkPrimitiveTopology topology,
                                size_t attributeCount);
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
  vulkan_buffer_view *bufferViews;
  vulkan_accessor *accessors;
} vulkan_scene;

void vulkan_scene_init(vulkan_scene *self, size_t nodesCount, size_t bufferViewsCount,
                       size_t accessorsCount);
void vulkan_scene_deinit(vulkan_scene *self);
void vulkan_scene_debug_print(vulkan_scene *self);

void parse_gltf_file(vulkan_scene *scene, platform_path gltfPath);

#include "../codegen/scene.h"

#endif /* !VULKAN_SCENE_H */
