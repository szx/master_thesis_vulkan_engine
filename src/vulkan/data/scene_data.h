/* Basic scene data.
 * glTF parsing and loading scene from asset database. */
#pragma once

#include "../../data/data.h"
#include "camera.h"

typedef struct vulkan_scene_data vulkan_scene_data;

/// Contains index buffer, interleaved vertex stream and topology of the part of the mesh.
typedef struct vulkan_primitive_data {
  VkPrimitiveTopology topology;
  uint32_t vertexCount;
  UT_array *positions; /// vec3
  UT_array *normals;   /// vec3
  UT_array *colors;    /// vec3
  UT_array *texCoords; /// vec2
  // TODO: LOD index buffers.
  UT_array *indices; /// uint32_t
  data_key hash;     /// Hash, used to prevent duplicates in asset database.
} vulkan_primitive_data;

typedef size_t vulkan_primitive_data_index;

void vulkan_primitive_data_init(vulkan_primitive_data *primitive);
void vulkan_primitive_data_deinit(vulkan_primitive_data *primitive);
void vulkan_primitive_data_debug_print(vulkan_primitive_data *primitive);

typedef struct vulkan_mesh_data {
  UT_array *primitives; /// vulkan_primitive_data_index array.
  data_key hash;        /// Hash, used to prevent duplicates in asset database.
} vulkan_mesh_data;

void vulkan_mesh_data_init(vulkan_mesh_data *mesh);
void vulkan_mesh_data_deinit(vulkan_mesh_data *mesh);
void vulkan_mesh_data_debug_print(vulkan_mesh_data *mesh, vulkan_scene_data *sceneData);

typedef struct vulkan_node_data {
  vulkan_mesh_data mesh;
  mat4 transform;
  // TODO: child nodes
  data_key hash; /// Hash, used to prevent duplicates in asset database.
} vulkan_node_data;

void vulkan_node_data_init(vulkan_node_data *node);
void vulkan_node_data_deinit(vulkan_node_data *node);
void vulkan_node_data_debug_print(vulkan_node_data *node, vulkan_scene_data *sceneData);

/// Describes a scene.
/// Does not create any Vulkan objects (see vulkan_scene).
typedef struct vulkan_scene_data {
  UT_string *name;
  UT_array *primitives; /// vulkan_primitive_data array, all primitives used by scene.
  UT_array *nodes;      /// vulkan_node_data array, all nodes in scene.
  UT_array *cameras;    /// vulkan_camera array, all cameras in scene
  bool dirty;           /// True if scene data updated on CPU.
  data_key hash;        /// Hash, used to prevent duplicates in asset database.
} vulkan_scene_data;

vulkan_scene_data *vulkan_scene_data_create(UT_string *name);
void vulkan_scene_data_destroy(vulkan_scene_data *sceneData);
void vulkan_scene_data_debug_print(vulkan_scene_data *sceneData);

/// Adds new primitive. Returns integer index.
vulkan_primitive_data_index vulkan_scene_data_add_primitive(vulkan_scene_data *sceneData,
                                                            vulkan_primitive_data primitive);

/* asset pipeline */
vulkan_scene_data *vulkan_scene_data_create_with_gltf_file(UT_string *sceneName,
                                                           UT_string *gltfPath);
vulkan_scene_data *vulkan_scene_data_create_with_asset_db(data_asset_db *assetDb,
                                                          UT_string *sceneName);
