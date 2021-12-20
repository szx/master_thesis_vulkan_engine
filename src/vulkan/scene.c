#include "scene.h"
#include "../codegen/scene.c"

void gltf_buffer_view_init(gltf_buffer_view *bufferView, char *name, size_t offset, size_t size,
                           size_t stride, vulkan_geometry_buffer *buffer) {
  bufferView->name = strdup(name);
  bufferView->offset = offset;
  bufferView->size = size;
  bufferView->stride = stride;
  bufferView->buffer = buffer;
}

void gltf_buffer_view_deinit(gltf_buffer_view *bufferView) {
  free(bufferView->name);
  bufferView->name = "FREE";
}

void gltf_accessor_init(gltf_accessor *accessor, char *name, size_t offset, size_t size,
                        size_t stride, gltf_buffer_view *bufferView) {
  accessor->name = strdup(name);
  accessor->offset = offset;
  accessor->count = size;
  accessor->stride = stride;
  accessor->bufferView = bufferView;
}
void gltf_accessor_deinit(gltf_accessor *accessor) {
  free(accessor->name);
  accessor->name = "FREE";
}

void gltf_attribute_init(gltf_attribute *attribute, char *name, vulkan_attribute_type type,
                         gltf_accessor *accessor) {
  assert(name != NULL);
  attribute->name = strdup(name);
  attribute->type = type;
  attribute->accessor = accessor;
}

void gltf_attribute_deinit(gltf_attribute *attribute) {
  free(attribute->name);
  attribute->name = 0;
  attribute->accessor = NULL;
}

int gltf_attribute_compare(const void *s1, const void *s2) {
  gltf_attribute *a1 = (gltf_attribute *)s1;
  gltf_attribute *a2 = (gltf_attribute *)s2;
  return (int)a1->type - (int)a2->type;
}

void vulkan_mesh_primitive_init(vulkan_mesh_primitive *primitive, VkPrimitiveTopology topology,
                                vulkan_attribute_type vertexTypes, vulkan_index_type indexType) {
  primitive->topology = topology;
  primitive->vertexCount = 0;
  primitive->vertexTypes = vertexTypes;
  primitive->vertexStride = vertex_types_to_vertex_stride(primitive->vertexTypes);
  const UT_icd vertexIcd = {primitive->vertexStride, NULL, NULL, NULL};
  utarray_new(primitive->vertexStream, &vertexIcd);
  primitive->indexCount = 0;
  primitive->indexType = indexType;
  primitive->indexStride = index_type_to_index_stride(primitive->indexType);
  const UT_icd indexIcd = {primitive->indexStride, NULL, NULL, NULL};
  utarray_new(primitive->indexBuffer, &indexIcd);
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
  scene->bufferViews = alloc_struct_array(gltf_buffer_view, bufferViewsCount);
  init_struct_array(scene->bufferViews);
  scene->accessors = alloc_struct_array(gltf_accessor, accessorsCount);
  init_struct_array(scene->accessors);
}

void vulkan_scene_deinit(vulkan_scene *self) {
  dealloc_struct(self->nodes);
  dealloc_struct(self->geometryBuffer);
  dealloc_struct(self->bufferViews);
  dealloc_struct(self->accessors);
}

void vulkan_scene_debug_print(vulkan_scene *self) {
  log_debug("SCENE:\n");
  log_debug("geometryBuffer: %d\n", self->geometryBuffer->dataSize);
  for (size_t i = 0; i < count_struct_array(self->nodes); i++) {
    log_debug("node:\n");
    vulkan_node *node = &self->nodes[i];
    glm_mat4_print(node->modelMat, stderr);
    vulkan_mesh *mesh = &node->mesh;
    for (size_t j = 0; j < count_struct_array(mesh->primitives); j++) {
      vulkan_mesh_primitive *primitive = &mesh->primitives[j];
      log_debug("  primitive %d: %s\n", j, VkPrimitiveTopology_debug_str(primitive->topology));
      log_debug("    index: %s stride=%d count=%d\n",
                vulkan_index_type_debug_str(primitive->indexType), primitive->indexStride,
                primitive->indexCount);
      void *index = NULL;
      while ((index = utarray_next(primitive->indexBuffer, index))) {
        if (primitive->indexType == vulkan_index_type_uint16) {
          log_debug("      %hu\n", *(uint16_t *)index);
        } else {
          log_debug("      %u\n", *(uint32_t *)index);
        }
      }
      log_debug("    vertex: stride=%d count=%d \n", primitive->vertexStride,
                primitive->vertexCount);
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

gltf_buffer_view *find_buffer_view(gltf_buffer_view *bufferViews,
                                   cgltf_buffer_view *cgltfBufferView) {
  for (size_t i = 0; i < count_struct_array(bufferViews); i++) {
    gltf_buffer_view *bufferView = &bufferViews[i];
    if (bufferView->offset == cgltfBufferView->offset &&
        bufferView->size == cgltfBufferView->size &&
        bufferView->stride == cgltfBufferView->stride) {
      return bufferView;
    }
  }
  assert(0);
  return NULL;
}

gltf_accessor *find_accessor(gltf_accessor *accessors, gltf_buffer_view *bufferViews,
                             cgltf_accessor *cgltfAccessor) {
  for (size_t i = 0; i < count_struct_array(accessors); i++) {
    gltf_accessor *accessor = &accessors[i];
    gltf_buffer_view *bufferView = find_buffer_view(bufferViews, cgltfAccessor->buffer_view);
    if (accessor->offset == cgltfAccessor->offset && accessor->count == cgltfAccessor->count &&
        accessor->stride == cgltfAccessor->stride &&
        strcmp(accessor->bufferView->name, bufferView->name) == 0) {
      return accessor;
    }
  }
  assert(0);
  return NULL;
}

gltf_attribute parse_attribute_accessor(cgltf_attribute *cgltfAttribute,
                                        gltf_buffer_view *bufferViews, gltf_accessor *accessors) {
  assert(cgltfAttribute != NULL);
  assert(cgltfAttribute->type == cgltf_attribute_type_position ||
         cgltfAttribute->type == cgltf_attribute_type_normal ||
         cgltfAttribute->type == cgltf_attribute_type_texcoord ||
         cgltfAttribute->type == cgltf_attribute_type_tangent);
  cgltf_accessor *cgltfAccessor = cgltfAttribute->data;

  size_t accessorOffset = cgltfAccessor->offset;
  size_t accessorCount = cgltfAccessor->count;
  size_t accessorStride = cgltfAccessor->stride;
  cgltf_buffer_view *cgltfBufferView = cgltfAccessor->buffer_view;
  gltf_buffer_view *bufferView = find_buffer_view(bufferViews, cgltfBufferView);
  vulkan_attribute_type type = cgltf_to_vulkan_attribute_type(cgltfAttribute->type);
  gltf_accessor *accessor = find_accessor(accessors, bufferViews, cgltfAccessor);
  gltf_attribute attribute;
  gltf_attribute_init(&attribute, cgltfAttribute->name, type, accessor);
  return attribute;
}

vulkan_node parse_cgltf_node(cgltf_node *cgltfNode, vulkan_geometry_buffer *geometryBuffer,
                             gltf_buffer_view *bufferViews, gltf_accessor *accessors) {
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
    gltf_accessor *indices = find_accessor(accessors, bufferViews, cgltfIndices);
    vulkan_index_type indexType = index_stride_to_index_type(indices->stride);
    uint32_t indexCount = indices->count;
    vulkan_attribute_type vertexTypes = 0;
    uint32_t vertexCount = 0;
    for (size_t attributeIdx = 0; attributeIdx < cgltfPrimitive->attributes_count; attributeIdx++) {
      cgltf_attribute *cgltfAttribute = &cgltfPrimitive->attributes[attributeIdx];
      gltf_attribute attribute = parse_attribute_accessor(cgltfAttribute, bufferViews, accessors);
      vertexTypes |= attribute.type;
      if (vertexCount == 0) {
        vertexCount = attribute.accessor->count;
      }
      verify(vertexCount == attribute.accessor->count);
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

void parse_gltf_file(vulkan_scene *scene, platform_path gltfPath) {
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
  // parse geometry buffer
  assert(data->buffers_count == 1);
  cgltf_buffer *cgltfBuffer = &data->buffers[0];
  scene->geometryBuffer = alloc_struct(vulkan_geometry_buffer);
  init_struct(scene->geometryBuffer, vulkan_geometry_buffer_init, cgltfBuffer->uri,
              cgltfBuffer->data, cgltfBuffer->size);
  // parse buffer views
  char *name = malloc(sizeof(char) * (255 + (int)log10(data->buffer_views_count)));
  for (size_t i = 0; i < bufferViewsCount; i++) {
    sprintf(name, "bufferView%d", (int)i);
    cgltf_buffer_view *cgltfBufferView = &data->buffer_views[i];
    assert(cgltfBufferView->has_meshopt_compression == false);
    gltf_buffer_view bufferView;
    gltf_buffer_view_init(&bufferView, name, cgltfBufferView->offset, cgltfBufferView->size,
                          cgltfBufferView->stride, scene->geometryBuffer);
    scene->bufferViews[i] = bufferView;
  }
  // parse accessors
  for (size_t i = 0; i < accessorsCount; i++) {
    sprintf(name, "accessor%d", (int)i);
    cgltf_accessor *cgltfAccessor = &data->accessors[i];
    gltf_buffer_view *bufferView = find_buffer_view(scene->bufferViews, cgltfAccessor->buffer_view);
    gltf_accessor accessor;
    gltf_accessor_init(&accessor, name, cgltfAccessor->offset, cgltfAccessor->count,
                       cgltfAccessor->stride, bufferView);
    scene->accessors[i] = accessor;
  }
  free(name);
  // parse nodes
  for (size_t nodeIdx = 0; nodeIdx < nodesCount; nodeIdx++) {
    vulkan_node node = parse_cgltf_node(cgltfScene->nodes[nodeIdx], scene->geometryBuffer,
                                        scene->bufferViews, scene->accessors);
    scene->nodes[nodeIdx] = node;
  }
  cgltf_free(data);
}
