#include "scene_data.h"

void vulkan_primitive_init(vulkan_primitive *primitive, VkPrimitiveTopology topology) {
  primitive->topology = topology;
  primitive->vertexCount = 0;
  utarray_alloc(primitive->positions, sizeof(vec3));
  utarray_alloc(primitive->normals, sizeof(vec3));
  utarray_alloc(primitive->colors, sizeof(vec3));
  utarray_alloc(primitive->texCoords, sizeof(vec2));

  primitive->indexCount = 0;
  primitive->indexType = vulkan_index_type_uint32;
  primitive->indexStride = index_type_to_index_stride(primitive->indexType);
  utarray_alloc(primitive->indices, primitive->indexStride);

  primitive->hash = 0;
}

void vulkan_primitive_deinit(vulkan_primitive *primitive) {
  utarray_free(primitive->positions);
  utarray_free(primitive->normals);
  utarray_free(primitive->colors);
  utarray_free(primitive->texCoords);
  utarray_free(primitive->indices);
}

void vulkan_mesh_init(vulkan_mesh *mesh, size_t primitiveCount) {
  core_array_alloc(mesh->primitives, primitiveCount);
}

void vulkan_mesh_deinit(vulkan_mesh *mesh) {
  core_array_foreach(mesh->primitives, vulkan_primitive * primitive,
                     { vulkan_primitive_deinit(primitive); });
  core_array_dealloc(mesh->primitives);
}

void vulkan_node_init(vulkan_node *node, vulkan_mesh mesh, mat4 transform, hash_t hash) {
  node->mesh = mesh;
  glm_mat4_copy(transform, node->transform);
  node->hash = hash;
}

void vulkan_node_deinit(vulkan_node *node) { vulkan_mesh_deinit(&node->mesh); }

void vulkan_node_debug_print(vulkan_node *node) {
  log_debug("node:\n");
  glm_mat4_print(node->transform, stderr);
  log_debug("  hash=%zu", node->hash);
  vulkan_mesh *mesh = &node->mesh;
  for (size_t j = 0; j < core_array_count(mesh->primitives); j++) {
    vulkan_primitive *primitive = &mesh->primitives.ptr[j];
    log_debug("  primitive %d: %s\n", j, VkPrimitiveTopology_debug_str(primitive->topology));
    log_debug("    index: %s stride=%d count=%d\n",
              vulkan_index_type_debug_str(primitive->indexType), primitive->indexStride,
              primitive->indexCount);
    void *index = NULL;
    while ((index = utarray_next(primitive->indices, index))) {
      log_debug("      %u\n", *(uint32_t *)index);
    }
    log_debug("    vertices: count=%d\n", primitive->vertexCount, primitive->vertexCount);
    uint32_t vertexIdx = 0;
    vec3 *position = NULL;
    while ((position = utarray_next(primitive->positions, position))) {
      log_debug("        position %d: %f %f %f\n", vertexIdx++, (*position)[0], (*position)[1],
                (*position)[2]);
    }
    vertexIdx = 0;
    vec3 *normal = NULL;
    while ((normal = utarray_next(primitive->normals, normal))) {
      log_debug("        normal %d: %f %f %f\n", vertexIdx++, (*normal)[0], (*normal)[1],
                (*normal)[2]);
    }
    vertexIdx = 0;
    vec3 *color = NULL;
    while ((color = utarray_next(primitive->colors, color))) {
      log_debug("        color %d: %f %f %f\n", vertexIdx++, (*color)[0], (*color)[1], (*color)[2]);
    }
    vertexIdx = 0;
    vec2 *texCoord = NULL;
    while ((texCoord = utarray_next(primitive->texCoords, texCoord))) {
      log_debug("        texCoord %d: %f %f\n", vertexIdx++, (*texCoord)[0], (*texCoord)[1]);
    }
    log_debug("    hash=%zu", primitive->hash);
  }
}

vulkan_scene_data *vulkan_scene_data_create(size_t nodesCount, UT_string *name) {
  vulkan_scene_data *sceneData = core_alloc(sizeof(vulkan_scene_data));
  utstring_new(sceneData->name);
  utstring_concat(sceneData->name, name);
  core_array_alloc(sceneData->nodes, nodesCount);
  sceneData->camera = vulkan_camera_create();
  sceneData->dirty = true;
  return sceneData;
}

void vulkan_scene_data_destroy(vulkan_scene_data *sceneData) {
  utstring_free(sceneData->name);
  core_array_foreach(sceneData->nodes, vulkan_node * node, { vulkan_node_deinit(node); });
  core_array_dealloc(sceneData->nodes);
  vulkan_camera_destroy(sceneData->camera);
  core_free(sceneData);
}

void vulkan_scene_data_debug_print(vulkan_scene_data *sceneData) {
  log_debug("SCENE_DATA:\n");
  for (size_t i = 0; i < core_array_count(sceneData->nodes); i++) {
    vulkan_node *node = &sceneData->nodes.ptr[i];
    vulkan_node_debug_print(node);
  }
}

/* vulkan_scene_data_create_with_gltf_file: */

VkPrimitiveTopology cgltf_primitive_type_vulkan_topology(cgltf_primitive_type type) {
  switch (type) {
  case cgltf_primitive_type_points:
    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
  case cgltf_primitive_type_lines:
    return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  case cgltf_primitive_type_line_strip:
    return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
  case cgltf_primitive_type_triangles:
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  case cgltf_primitive_type_triangle_strip:
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
  case cgltf_primitive_type_triangle_fan:
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
  case cgltf_primitive_type_line_loop:
  default:
    return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
  }
}

vulkan_attribute_type cgltf_to_vulkan_attribute_type(cgltf_attribute_type type) {
  switch (type) {
  case cgltf_attribute_type_position:
    return PositionAttribute;
  case cgltf_attribute_type_normal:
    return NormalAttribute;
  case cgltf_attribute_type_tangent:
    return TangentAttribute;
  case cgltf_attribute_type_texcoord:
    return TexCoordAttribute;
  case cgltf_attribute_type_color:
    return ColorAttribute;
  case cgltf_attribute_type_joints:
  case cgltf_attribute_type_weights:
  case cgltf_attribute_type_invalid:
  default:
    return UnknownAttribute;
  }
}

vulkan_node parse_cgltf_node(cgltf_node *cgltfNode) {
  assert(cgltfNode->extras.start_offset == 0 && cgltfNode->extras.end_offset == 0);
  assert(cgltfNode->camera == NULL);
  assert(cgltfNode->light == NULL);
  assert(cgltfNode->weights_count == 0);
  assert(cgltfNode->children_count == 0);
  cgltf_mesh *cgltfMesh = cgltfNode->mesh;
  assert(cgltfMesh != NULL);
  assert(cgltfMesh->extras.start_offset == 0 && cgltfMesh->extras.end_offset == 0);
  assert(cgltfMesh->target_names_count == 0);

  vulkan_mesh mesh;
  vulkan_mesh_init(&mesh, cgltfMesh->primitives_count);
  HASH_START(meshHash)

  // parse primitives
  for (size_t primitiveIdx = 0; primitiveIdx < cgltfMesh->primitives_count; primitiveIdx++) {
    cgltf_primitive *cgltfPrimitive = &cgltfMesh->primitives[primitiveIdx];
    assert(cgltfPrimitive->has_draco_mesh_compression == false);
    assert(cgltfPrimitive->targets_count == 0);
    assert(cgltfPrimitive->mappings_count == 0);
    cgltf_accessor *cgltfIndices = cgltfPrimitive->indices;
    assert(cgltfIndices != NULL);

    VkPrimitiveTopology topology = cgltf_primitive_type_vulkan_topology(cgltfPrimitive->type);
    vulkan_index_type indexType = index_stride_to_index_type(cgltfIndices->stride);
    if (indexType != vulkan_index_type_uint32) {
      indexType = vulkan_index_type_uint32;
    }
    uint32_t indexCount = cgltfIndices->count;
    vulkan_attribute_type vertexAttributes = 0;
    uint32_t vertexCount = 0;
    for (size_t attributeIdx = 0; attributeIdx < cgltfPrimitive->attributes_count; attributeIdx++) {
      cgltf_attribute *cgltfAttribute = &cgltfPrimitive->attributes[attributeIdx];
      vulkan_attribute_type type = cgltf_to_vulkan_attribute_type(cgltfAttribute->type);
      vertexAttributes |= type;
      if (vertexCount == 0) {
        vertexCount = cgltfAttribute->data->count;
      }
      verify(vertexCount == cgltfAttribute->data->count);
    }

    vulkan_primitive primitive;
    vulkan_primitive_init(&primitive, topology);
    primitive.vertexCount = vertexCount;
    primitive.indexCount = indexCount;

    // read indices
    utarray_resize(primitive.indices, primitive.indexCount);
    for (size_t i = 0; i < primitive.indexCount; i++) {
      size_t indexValue = cgltf_accessor_read_index(cgltfIndices, i);
      void *outValue = utarray_eltptr(primitive.indices, i);
      *(uint32_t *)outValue = indexValue;
    }

    // read vertex attributes
    if ((vertexAttributes & PositionAttribute) != 0) {
      utarray_resize(primitive.positions, primitive.vertexCount);
    }
    if ((vertexAttributes & NormalAttribute) != 0) {
      utarray_resize(primitive.normals, primitive.vertexCount);
    }
    if ((vertexAttributes & ColorAttribute) != 0) {
      utarray_resize(primitive.colors, primitive.vertexCount);
    }
    if ((vertexAttributes & TexCoordAttribute) != 0) {
      utarray_resize(primitive.texCoords, primitive.vertexCount);
    }
    for (size_t i = 0; i < primitive.vertexCount; i++) {
      for (size_t attributeIdx = 0; attributeIdx < cgltfPrimitive->attributes_count;
           attributeIdx++) {
        cgltf_attribute *cgltfAttribute = &cgltfPrimitive->attributes[attributeIdx];
        vulkan_attribute_type type = cgltf_to_vulkan_attribute_type(cgltfAttribute->type);
        if (type == PositionAttribute) {
          verify(cgltf_accessor_read_float(cgltfAttribute->data, i,
                                           (float *)utarray_eltptr(primitive.positions, i), 3));
        }
        if (type == NormalAttribute) {
          verify(cgltf_accessor_read_float(cgltfAttribute->data, i,
                                           (float *)utarray_eltptr(primitive.normals, i), 3));
        }
        if (type == ColorAttribute) {
          verify(cgltf_accessor_read_float(cgltfAttribute->data, i,
                                           (float *)utarray_eltptr(primitive.colors, i), 3));
        }
        if (type == TexCoordAttribute) {
          verify(cgltf_accessor_read_float(cgltfAttribute->data, i,
                                           (float *)utarray_eltptr(primitive.texCoords, i), 2));
        }
      }
    }

    // TODO: Currently primitive hash is produced using all topology + indices + vertex attributes.
    //       This eliminates duplicated primitives with EXACTLY the same structure, but still
    //       results in e.g. nodes with same vertex attributes but different indices are stored as
    //       separate entries in asset database, even if they share vertex attributes.
    //       It could be optimized with separate hashes and tables for each vertex attribute and
    //       foreign keys in primitive table. Not sure if its worth additional work.
    HASH_START(primitiveState)
    HASH_UPDATE(primitiveState, &primitive.topology, sizeof(primitive.topology))
#define HASH_UPDATE_FUNC(_i, _array)                                                               \
  HASH_UPDATE(primitiveState, utarray_front(_array), utarray_size(_array))
    MACRO_FOREACH(HASH_UPDATE_FUNC, primitive.indices, primitive.positions, primitive.normals,
                  primitive.colors, primitive.texCoords)
#undef HASH_UPDATE_FUNC
    HASH_DIGEST(primitiveState, primitive.hash)
    HASH_END(primitiveState)

    // add primitive hash to mesh hash
    HASH_UPDATE(meshHash, &primitive.hash, sizeof(primitive.hash));

    // cgltf_material* material;
    mesh.primitives.ptr[primitiveIdx] = primitive;
  }

  HASH_DIGEST(meshHash, mesh.hash)
  HASH_END(meshHash)

  vulkan_node node;
  vulkan_node_init(&node, mesh, GLM_MAT4_IDENTITY, 0);
  // TODO: Animation, will probably require cgltf_node_transform_local().
  cgltf_node_transform_world(cgltfNode, (float *)node.transform);
  // TODO: child nodes
  HASH_START(nodeState)
  HASH_UPDATE(nodeState, &node.transform, sizeof(node.transform))
  HASH_UPDATE(nodeState, &node.mesh.hash, sizeof(node.mesh.hash))
  HASH_DIGEST(nodeState, node.hash)
  HASH_END(nodeState)

  return node;
}

vulkan_scene_data *vulkan_scene_data_create_with_gltf_file(UT_string *gltfName,
                                                           UT_string *gltfPath) {
  cgltf_options options = {0};
  cgltf_data *data = NULL;
  cgltf_result result = cgltf_result_success;
  result = cgltf_parse_file(&options, utstring_body(gltfPath), &data);
  if (result != cgltf_result_success) {
    panic("failed to parse gltf file");
  }
  result = cgltf_load_buffers(&options, data, utstring_body(gltfPath));
  if (result != cgltf_result_success) {
    panic("failed to load gltf buffers");
  }

  assert(data->scenes_count == 1);
  cgltf_scene *cgltfScene = &data->scene[0];
  size_t nodesCount = cgltfScene->nodes_count;
  size_t bufferViewsCount = data->buffer_views_count;
  size_t accessorsCount = data->accessors_count;
  vulkan_scene_data *sceneData = vulkan_scene_data_create(nodesCount, gltfName);

  for (size_t nodeIdx = 0; nodeIdx < nodesCount; nodeIdx++) {
    vulkan_node node = parse_cgltf_node(cgltfScene->nodes[nodeIdx]);
    sceneData->nodes.ptr[nodeIdx] = node;
  }
  // TODO: cameras from gltf file
  cgltf_free(data);

  return sceneData;
}

vulkan_scene_data *vulkan_scene_data_create_with_asset_db(data_asset_db *assetDb,
                                                          UT_string *sceneName) {
  vulkan_scene_data *sceneData = NULL;

  data_hash_array nodeHashArray =
      data_asset_db_select_scene_nodes_hash_array(assetDb, utstring_blob(sceneName));
  UT_array *nodeHashes = hash_array_utarray(nodeHashArray);

  sceneData = vulkan_scene_data_create(utarray_len(nodeHashes), sceneName);

  data_blob cameraBlob =
      data_asset_db_select_scene_cameras_blob(assetDb, utstring_blob(sceneData->name));
  vulkan_camera_deserialize(sceneData->camera, cameraBlob);

  hash_t *nodeHash = NULL;
  size_t nodeIdx = 0;
  while ((nodeHash = (utarray_next(nodeHashes, nodeHash)))) {
    /* node data */
    data_mat4 transform = data_asset_db_select_node_transform_mat4(assetDb, hash_blob(*nodeHash));
    // TODO: Chlid nodes.
    data_hash meshHash = data_asset_db_select_node_mesh_hash(assetDb, hash_blob(*nodeHash));
    vulkan_node node;

    /* mesh data */
    data_hash_array primitiveHashArray =
        data_asset_db_select_mesh_primitives_hash_array(assetDb, hash_blob(meshHash));
    UT_array *primitiveHashes = hash_array_utarray(primitiveHashArray);
    vulkan_mesh mesh;
    vulkan_mesh_init(&mesh, utarray_len(primitiveHashes));

    /* primitive data */
    hash_t *primitiveHash = NULL;
    size_t primitiveIdx = 0;
    while ((primitiveHash = (utarray_next(primitiveHashes, primitiveHash)))) {
      VkPrimitiveTopology topology =
          data_asset_db_select_primitive_topology_int(assetDb, hash_blob(*primitiveHash));
      vulkan_primitive primitive;
      vulkan_primitive_init(&primitive, topology);
      // HIRO read attributes
      // HIRO just rewrite scene_data, remove core_array, SoA for primitives, meshes, nodes.
      mesh.primitives.ptr[primitiveIdx++] = primitive;
    }

    /* add node */
    vulkan_node_init(&node, mesh, transform.value, *nodeHash);
    sceneData->nodes.ptr[nodeIdx++] = node;
  }
  // HIRO: load from asset_db

  /*vulkan_scene_data *sceneData = vulkan_scene_data_create(nodesCount);

  for (size_t nodeIdx = 0; nodeIdx < nodesCount; nodeIdx++) {
    vulkan_node node = parse_cgltf_node(cgltfScene->nodes[nodeIdx]);
    sceneData->nodes.ptr[nodeIdx] = node;
    HASH_UPDATE(sceneState, &node.hash, sizeof(node.hash))
  }

  return sceneData;*/
  panic("vulkan_scene_data_create_with_asset_db not implemented");
  return sceneData;
}
