/* Vulkan scene and glTF parsing */

#ifndef VULKAN_SCENE_H
#define VULKAN_SCENE_H

#include "../platform.h"

#include "functions.h"

typedef struct vulkan_geometry_buffer {
  char *name;
  uint8_t *data;
  size_t dataSize;
} vulkan_geometry_buffer;

vulkan_geometry_buffer vulkan_geometry_buffer_init(char *name, uint8_t *data,
                                                   size_t size);
void vulkan_geometry_buffer_free(vulkan_geometry_buffer *self);

typedef struct vulkan_buffer_view {
  char *name;
  size_t offset;
  size_t size;
  size_t stride;
  vulkan_geometry_buffer *buffer;
} vulkan_buffer_view;

vulkan_buffer_view vulkan_buffer_view_init(char *name, size_t offset,
                                           size_t size, size_t stride,
                                           vulkan_geometry_buffer *buffer);
void vulkan_buffer_view_free(vulkan_buffer_view *self);

#define P
#define T vulkan_buffer_view
#include "vec.h" // vec_vulkan_buffer_view

typedef struct vulkan_accessor {
  char *name;
  size_t offset;
  size_t size;
  size_t stride;
  vulkan_buffer_view *bufferView;
} vulkan_accessor;

vulkan_accessor vulkan_accessor_init(char *name, size_t offset, size_t size,
                                     size_t stride,
                                     vulkan_buffer_view *bufferView);
void vulkan_accessor_free(vulkan_accessor *self);

#define P
#define T vulkan_accessor
#include "vec.h" // vec_vulkan_accessor

typedef enum vulkan_attribute_type {
  UnknownAttribute,
  PositionAttribute,
  NormalAttribute,
  TangentAttribute,
  TexcoordAttribute,
  ColorAttribute
} vulkan_attribute_type;

typedef struct vulkan_attribute {
  char *name;
  vulkan_attribute_type type;
  size_t index;
  vulkan_accessor *accessor;
} vulkan_attribute;

vulkan_attribute vulkan_attribute_init(char *name, vulkan_attribute_type type,
                                       size_t index, vulkan_accessor *accessor);
void vulkan_attribute_free(vulkan_attribute *self);
vulkan_attribute vulkan_attribute_copy(vulkan_attribute *self);

#define T vulkan_attribute
#include "vec.h" // vec_vulkan_attribute

typedef struct vulkan_mesh_primitive {
  VkPrimitiveTopology topology;
  vec_vulkan_attribute attributes;
  vulkan_accessor *indices;
} vulkan_mesh_primitive;

vulkan_mesh_primitive vulkan_mesh_primitive_init(VkPrimitiveTopology topology);
void vulkan_mesh_primitive_free(vulkan_mesh_primitive *self);
vulkan_mesh_primitive vulkan_mesh_primitive_copy(vulkan_mesh_primitive *self);
void vulkan_mesh_primitive_add_attribute(vulkan_mesh_primitive *self,
                                         vulkan_attribute attribute);

#define T vulkan_mesh_primitive
#include "vec.h" // vec_vulkan_mesh_primitive

typedef struct vulkan_mesh {
  vec_vulkan_mesh_primitive primitives;
} vulkan_mesh;

vulkan_mesh vulkan_mesh_init();
void vulkan_mesh_free(vulkan_mesh *self);
vulkan_mesh vulkan_mesh_copy(vulkan_mesh *self);
void vulkan_mesh_add_mesh_primitive(vulkan_mesh *self,
                                    vulkan_mesh_primitive meshPrimitive);

typedef struct vulkan_node {
  vulkan_mesh mesh;
  mat4 transform;
} vulkan_node;

vulkan_node vulkan_node_init(vulkan_mesh mesh);
void vulkan_node_free(vulkan_node *self);
vulkan_node vulkan_node_copy(vulkan_node *self);

#define T vulkan_node
#include "vec.h" // vec_vulkan_node

// Describes a scene.
// Does not create any Vulkan objects (see vulkan_render_context).
typedef struct vulkan_scene {
  vec_vulkan_node nodes;
  vulkan_geometry_buffer geometryBuffer;
  vec_vulkan_buffer_view bufferViews;
  vec_vulkan_accessor accessors;
} vulkan_scene;

vulkan_scene vulkan_scene_init();
void vulkan_scene_free(vulkan_scene *self);
void vulkan_scene_add_node(vulkan_scene *self, vulkan_node node);
void vulkan_scene_debug_print(vulkan_scene *self);

vulkan_scene parse_gltf_file(platform_path gltfPath);

#include "../codegen/scene.h"

#endif /* !VULKAN_SCENE_H */
