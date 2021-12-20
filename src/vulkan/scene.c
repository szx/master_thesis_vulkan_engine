#include "scene.h"
#include "../codegen/scene.c"

void vulkan_mesh_primitive_init(vulkan_mesh_primitive *primitive, VkPrimitiveTopology topology,
                                vulkan_attribute_type vertexTypes, vulkan_index_type indexType) {
  primitive->topology = topology;
  primitive->vertexCount = 0;
  primitive->vertexTypes = vertexTypes;
  primitive->vertexStride = vertex_types_to_vertex_stride(primitive->vertexTypes);
  const UT_icd vertexIcd = {primitive->vertexStride, NULL, NULL, NULL};
  utarray_new(primitive->vertexStream, &vertexIcd);
  primitive->vertexStreamOffset = ~0;

  primitive->indexCount = 0;
  primitive->indexType = indexType;
  primitive->indexStride = index_type_to_index_stride(primitive->indexType);
  const UT_icd indexIcd = {primitive->indexStride, NULL, NULL, NULL};
  utarray_new(primitive->indexBuffer, &indexIcd);
  primitive->indexBufferOffset = ~0;
}

void vulkan_mesh_primitive_deinit(vulkan_mesh_primitive *primitive) {
  utarray_free(primitive->vertexStream);
  utarray_free(primitive->indexBuffer);
}

void vulkan_mesh_init(vulkan_mesh *mesh, size_t primitiveCount) {
  mesh->primitives = alloc_struct_array(vulkan_mesh_primitive, primitiveCount);
  init_struct_array(mesh->primitives);
}

void vulkan_mesh_deinit(vulkan_mesh *mesh) { dealloc_struct(mesh->primitives); }

void vulkan_node_init(vulkan_node *scene, vulkan_mesh mesh) {
  scene->mesh = mesh;
  glm_mat4_identity(scene->modelMat);
}

void vulkan_node_deinit(vulkan_node *self) { vulkan_mesh_deinit(&self->mesh); }

void vulkan_scene_init(vulkan_scene *scene, size_t nodesCount, size_t bufferViewsCount,
                       size_t accessorsCount) {
  scene->nodes = alloc_struct_array(vulkan_node, nodesCount);
  init_struct_array(scene->nodes);
  scene->geometryBuffer = alloc_struct(vulkan_geometry_buffer);
  init_struct(scene->geometryBuffer, vulkan_geometry_buffer_init);
}

void vulkan_scene_deinit(vulkan_scene *self) {
  dealloc_struct(self->nodes);
  dealloc_struct(self->geometryBuffer);
}

void vulkan_scene_debug_print(vulkan_scene *self) {
  log_debug("SCENE:\n");
  log_debug("geometryBuffer: %d\n", utarray_len(self->geometryBuffer->data));
  for (size_t i = 0; i < count_struct_array(self->nodes); i++) {
    log_debug("node:\n");
    vulkan_node *node = &self->nodes[i];
    glm_mat4_print(node->modelMat, stderr);
    vulkan_mesh *mesh = &node->mesh;
    for (size_t j = 0; j < count_struct_array(mesh->primitives); j++) {
      vulkan_mesh_primitive *primitive = &mesh->primitives[j];
      log_debug("  primitive %d: %s\n", j, VkPrimitiveTopology_debug_str(primitive->topology));
      log_debug("    index: %s stride=%d count=%d offset=%lu\n",
                vulkan_index_type_debug_str(primitive->indexType), primitive->indexStride,
                primitive->indexCount, primitive->indexBufferOffset);
      void *index = NULL;
      while ((index = utarray_next(primitive->indexBuffer, index))) {
        if (primitive->indexType == vulkan_index_type_uint16) {
          log_debug("      %hu\n", *(uint16_t *)index);
        } else {
          log_debug("      %u\n", *(uint32_t *)index);
        }
      }
      log_debug("    vertex: stride=%d count=%d offset=%lu\n", primitive->vertexStride,
                primitive->vertexCount, primitive->vertexStreamOffset);
      vulkan_vertex_stream_element *vertex = NULL;
      uint32_t vertexIdx = 0;
      while ((vertex = utarray_next(primitive->vertexStream, vertex))) {
        log_debug("      %d:", vertexIdx++);
        if ((primitive->vertexTypes & PositionAttribute) != 0) {
          log_debug("        position: %f %f %f\n", vertex->position[0], vertex->position[1],
                    vertex->position[2]);
        }
        if ((primitive->vertexTypes & NormalAttribute) != 0) {
          log_debug("        normal: %f %f %f\n", vertex->normal[0], vertex->normal[1],
                    vertex->normal[2]);
        }
        if ((primitive->vertexTypes & ColorAttribute) != 0) {
          log_debug("        color: %f %f %f\n", vertex->color[0], vertex->color[1],
                    vertex->color[2]);
        }
        if ((primitive->vertexTypes & TexCoordAttribute) != 0) {
          log_debug("        texCoord: %f %f\n", vertex->texCoord[0], vertex->texCoord[1]);
        }
      }
    }
  }
}

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

  for (size_t primitiveIdx = 0; primitiveIdx < cgltfMesh->primitives_count; primitiveIdx++) {
    cgltf_primitive *cgltfPrimitive = &cgltfMesh->primitives[primitiveIdx];
    assert(cgltfPrimitive->has_draco_mesh_compression == false);
    assert(cgltfPrimitive->targets_count == 0);
    assert(cgltfPrimitive->mappings_count == 0);
    cgltf_accessor *cgltfIndices = cgltfPrimitive->indices;
    assert(cgltfIndices != NULL);

    VkPrimitiveTopology topology = cgltf_primitive_type_vulkan_topology(cgltfPrimitive->type);
    vulkan_index_type indexType = index_stride_to_index_type(cgltfIndices->stride);
    uint32_t indexCount = cgltfIndices->count;
    vulkan_attribute_type vertexTypes = 0;
    uint32_t vertexCount = 0;
    for (size_t attributeIdx = 0; attributeIdx < cgltfPrimitive->attributes_count; attributeIdx++) {
      cgltf_attribute *cgltfAttribute = &cgltfPrimitive->attributes[attributeIdx];
      vulkan_attribute_type type = cgltf_to_vulkan_attribute_type(cgltfAttribute->type);
      vertexTypes |= type;
      if (vertexCount == 0) {
        vertexCount = cgltfAttribute->data->count;
      }
      verify(vertexCount == cgltfAttribute->data->count);
    }

    vulkan_mesh_primitive meshPrimitive;
    vulkan_mesh_primitive_init(&meshPrimitive, topology, vertexTypes, indexType);
    meshPrimitive.vertexCount = vertexCount;
    meshPrimitive.indexCount = indexCount;
    utarray_resize(meshPrimitive.indexBuffer, meshPrimitive.indexCount);
    for (size_t i = 0; i < meshPrimitive.indexCount; i++) {
      size_t indexValue = cgltf_accessor_read_index(cgltfIndices, i);
      void *outValue = utarray_eltptr(meshPrimitive.indexBuffer, i);
      if (meshPrimitive.indexType == vulkan_index_type_uint16) {
        *(uint16_t *)outValue = indexValue;
      } else {
        *(uint32_t *)outValue = indexValue;
      }
    }

    utarray_resize(meshPrimitive.vertexStream, meshPrimitive.vertexCount);
    for (size_t i = 0; i < meshPrimitive.vertexCount; i++) {
      vulkan_vertex_stream_element vertex;
      for (size_t attributeIdx = 0; attributeIdx < cgltfPrimitive->attributes_count;
           attributeIdx++) {
        cgltf_attribute *cgltfAttribute = &cgltfPrimitive->attributes[attributeIdx];
        vulkan_attribute_type type = cgltf_to_vulkan_attribute_type(cgltfAttribute->type);
        if (type == PositionAttribute) {
          verify(cgltf_accessor_read_float(cgltfAttribute->data, i, (float *)&vertex.position, 3));
        }
        if (type == NormalAttribute) {
          verify(cgltf_accessor_read_float(cgltfAttribute->data, i, (float *)&vertex.normal, 3));
        }
        if (type == ColorAttribute) {
          verify(cgltf_accessor_read_float(cgltfAttribute->data, i, (float *)&vertex.color, 3));
        }
        if (type == TexCoordAttribute) {
          verify(cgltf_accessor_read_float(cgltfAttribute->data, i, (float *)&vertex.texCoord, 2));
        }
      }
      vulkan_vertex_stream_element *outVertex =
          (vulkan_vertex_stream_element *)utarray_eltptr(meshPrimitive.vertexStream, i);
      *outVertex = vertex;
    }

    // cgltf_material* material;
    mesh.primitives[primitiveIdx] = meshPrimitive;
  }
  vulkan_node node;
  vulkan_node_init(&node, mesh);
  // TODO: Animation, will probably require cgltf_node_transform_local().
  cgltf_node_transform_world(cgltfNode, (float *)node.modelMat);
  return node;
}

void vulkan_scene_init_with_gltf_file(vulkan_scene *scene, platform_path gltfPath) {
  // read gltf file
  cgltf_options options = {0};
  cgltf_data *data = NULL;
  cgltf_result result = cgltf_result_success;
  result = cgltf_parse_file(&options, platform_path_c_str(&gltfPath), &data);
  if (result != cgltf_result_success) {
    panic("failed to parse gltf file");
  }
  result = cgltf_load_buffers(&options, data, platform_path_c_str(&gltfPath));
  if (result != cgltf_result_success) {
    panic("failed to load gltf buffers");
  }
  // parse scene
  assert(data->scenes_count == 1);
  cgltf_scene *cgltfScene = &data->scene[0];
  size_t nodesCount = cgltfScene->nodes_count;
  size_t bufferViewsCount = data->buffer_views_count;
  size_t accessorsCount = data->accessors_count;
  init_struct(scene, vulkan_scene_init, nodesCount, bufferViewsCount, accessorsCount);
  // parse nodes
  for (size_t nodeIdx = 0; nodeIdx < nodesCount; nodeIdx++) {
    vulkan_node node = parse_cgltf_node(cgltfScene->nodes[nodeIdx]);
    scene->nodes[nodeIdx] = node;
  }
  cgltf_free(data);

  // build geometry buffer
  vulkan_scene_build_geometry_buffer(scene);
}

void vulkan_scene_build_geometry_buffer(vulkan_scene *scene) {
  // TODO: Overlapping index buffers and vertex streams.
  // TODO: Free node resources after building scene.
  log_debug("vulkan_scene_build_geometry_buffer");
  vulkan_scene_debug_print(scene);
  UT_array *data = scene->geometryBuffer->data;
  assert(utarray_len(data) == 0);
  size_t paddingValue = 0;
  for (size_t nodeIdx = 0; nodeIdx < count_struct_array(scene->nodes); nodeIdx++) {
    log_debug("node:\n");
    vulkan_node *node = &scene->nodes[nodeIdx];
    vulkan_mesh *mesh = &node->mesh;
    for (size_t primIdx = 0; primIdx < count_struct_array(mesh->primitives); primIdx++) {
      vulkan_mesh_primitive *primitive = &mesh->primitives[primIdx];
      // index buffer
      size_t indexBufferOffset = utarray_len(data);
      if ((indexBufferOffset % primitive->indexStride) != 0) {
        size_t remainder = (indexBufferOffset % primitive->indexStride);
        indexBufferOffset += remainder;
        for (size_t i = 0; i < remainder; i++) {
          utarray_push_back(data, &paddingValue);
        }
      }
      primitive->indexBufferOffset = indexBufferOffset;
      size_t indexBufferSize = primitive->indexStride * primitive->indexCount;
      utarray_resize(data, utarray_len(data) + indexBufferSize);
      void *geometryData = utarray_eltptr(data, utarray_len(data) - indexBufferSize);
      void *indexData = utarray_front(primitive->indexBuffer);
      memcpy(geometryData, indexData, indexBufferSize);
      // vertex stream
      size_t vertexStreamOffset = utarray_len(data);
      if ((vertexStreamOffset % primitive->vertexStride) != 0) {
        // TODO: Is it too much? Should we align vertex streams?
        size_t remainder = (vertexStreamOffset % primitive->vertexStride);
        vertexStreamOffset += remainder;
        for (size_t i = 0; i < remainder; i++) {
          utarray_push_back(data, &paddingValue);
        }
      }
      primitive->vertexStreamOffset = vertexStreamOffset;
      size_t vertexStreamSize = primitive->vertexStride * primitive->vertexCount;
      utarray_resize(data, utarray_len(data) + vertexStreamSize);
      geometryData = utarray_eltptr(data, utarray_len(data) - vertexStreamSize);
      void *vertexData = utarray_front(primitive->vertexStream);
      memcpy(geometryData, vertexData, vertexStreamSize);
    }
  }
}
