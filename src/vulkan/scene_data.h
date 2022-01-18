/* Basic scene objects, glTF parsing and loading scene from asset database */
#pragma once

#include "../core/platform.h"
#include "camera.h"

/// Contains index buffer, interleaved vertex stream and topology of the part of the mesh.
typedef struct vulkan_mesh_primitive {
  VkPrimitiveTopology topology;
  uint32_t vertexCount;
  vulkan_attribute_type vertexAttributes;
  uint32_t vertexStride;       /// Calculated using vertexAttributes.
  UT_array *vertexStream;      /// vulkan_vertex_stream_element
  uint32_t vertexStreamOffset; /// Offset of vertex stream in geometry buffer.
  // TODO: LOD index buffers.
  uint32_t indexCount;
  vulkan_index_type indexType;
  uint32_t indexStride;       /// Calculated using indexType.
  UT_array *indexBuffer;      /// uint32_t
  uint32_t indexBufferOffset; /// Offset of index buffer in geometry buffer.

  XXH64_hash_t hash; /// Hash, used to prevent duplicates in asset database.
} vulkan_mesh_primitive;

void vulkan_mesh_primitive_init(vulkan_mesh_primitive *primitive, VkPrimitiveTopology topology,
                                vulkan_attribute_type vertexAttributes,
                                vulkan_index_type indexType);
void vulkan_mesh_primitive_deinit(vulkan_mesh_primitive *primitive);

typedef struct vulkan_mesh {
  core_array(vulkan_mesh_primitive) primitives;
} vulkan_mesh;

void vulkan_mesh_init(vulkan_mesh *mesh, size_t primitiveCount);
void vulkan_mesh_deinit(vulkan_mesh *mesh);

typedef struct vulkan_node {
  vulkan_mesh mesh;
  mat4 modelMat;
  // TODO: child nodes
} vulkan_node;

void vulkan_node_init(vulkan_node *scene, vulkan_mesh mesh);
void vulkan_node_deinit(vulkan_node *scene);

/// Describes a scene.
/// Does not create any Vulkan objects (see vulkan_scene).
typedef struct vulkan_scene_data {
  core_array(vulkan_node) nodes;
  vulkan_camera *camera;
  bool dirty; /// True if scene data updated on CPU.
} vulkan_scene_data;

vulkan_scene_data *vulkan_scene_data_create(size_t nodesCount);
void vulkan_scene_data_destroy(vulkan_scene_data *sceneData);
void vulkan_scene_data_debug_print(vulkan_scene_data *sceneData);

/// Loads scene data from glTF file.
/// To be used only by by asset pipeline.
vulkan_scene_data *vulkan_scene_data_create_with_gltf_file(UT_string *gltfPath);
