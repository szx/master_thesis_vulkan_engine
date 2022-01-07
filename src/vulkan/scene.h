/* Vulkan scene and glTF parsing */
#pragma once

#include "../core/platform.h"

#include "camera.h"
#include "device.h"
#include "functions.h"

/// Contains index buffer, interleaved vertex stream and topology of the part of the mesh.
typedef struct vulkan_mesh_primitive {
  VkPrimitiveTopology topology;
  uint32_t vertexCount;
  vulkan_attribute_type vertexTypes;
  uint32_t vertexStride;       /// Calculated using vertexTypes.
  UT_array *vertexStream;      /// vulkan_vertex_stream_element
  uint32_t vertexStreamOffset; /// Offset of vertex stream in geometry buffer.

  // TODO: LOD index buffers.
  uint32_t indexCount;
  vulkan_index_type indexType;
  uint32_t indexStride;       /// Calculated using indexType.
  UT_array *indexBuffer;      /// uint32_t or uint16_t
  uint32_t indexBufferOffset; /// Offset of index buffer in geometry buffer.
} vulkan_mesh_primitive;

void vulkan_mesh_primitive_init(vulkan_mesh_primitive *primitive, VkPrimitiveTopology topology,
                                vulkan_attribute_type vertexTypes, vulkan_index_type indexType);
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

// Describes a scene.
// Does not create any Vulkan objects (see vulkan_render_context).
typedef struct vulkan_scene {
  vulkan_device *vkd; // vulkan_device pointer
  core_array(vulkan_node) nodes;
  vulkan_geometry_buffer *geometryBuffer;
  vulkan_uniform_buffer *uniformBuffer;
  vulkan_camera *camera;
  // HIRO VK_EXT_vertex_input_dynamic_state is not supported by Intel.
  // HIRO VK_EXT_extended_dynamic_state allows VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT
  // HIRO Save and load scene from disk.
  bool dirty; /// True if scene data updated on CPU.
} vulkan_scene;

void vulkan_scene_init(vulkan_scene *self, vulkan_device *vkd, size_t nodesCount,
                       size_t bufferViewsCount, size_t accessorsCount);
void vulkan_scene_deinit(vulkan_scene *self);
void vulkan_scene_debug_print(vulkan_scene *self);
vulkan_scene *vulkan_scene_create_with_gltf_file(vulkan_device *vkd, UT_string *gltfPath);
void vulkan_scene_destroy(vulkan_scene *scene);
void vulkan_scene_build_geometry_buffer(vulkan_scene *scene); // HIRO remove build geometry buffer
/// Update scene data on CPU. Can make camera and uniform buffer dirty.
void vulkan_scene_update_data(vulkan_scene *scene);
void vulkan_scene_send_to_device(vulkan_scene *scene);

