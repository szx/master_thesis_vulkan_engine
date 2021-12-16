#include "scene.h"
#include "../codegen/scene.c"

void vulkan_buffer_view_init(vulkan_buffer_view *bufferView, char *name, size_t offset, size_t size,
                             size_t stride, vulkan_geometry_buffer *buffer) {
  bufferView->name = strdup(name);
  bufferView->offset = offset;
  bufferView->size = size;
  bufferView->stride = stride;
  bufferView->buffer = buffer;
}

void vulkan_buffer_view_deinit(vulkan_buffer_view *bufferView) {
  free(bufferView->name);
  bufferView->name = "FREE";
}

void vulkan_accessor_init(vulkan_accessor *accessor, char *name, size_t offset, size_t size,
                          size_t stride, vulkan_buffer_view *bufferView) {
  accessor->name = strdup(name);
  accessor->offset = offset;
  accessor->count = size;
  accessor->stride = stride;
  accessor->bufferView = bufferView;
}
void vulkan_accessor_deinit(vulkan_accessor *accessor) {
  free(accessor->name);
  accessor->name = "FREE";
}

void vulkan_attribute_init(vulkan_attribute *attribute, char *name, vulkan_attribute_type type,
                           size_t index, vulkan_accessor *accessor) {
  assert(name != NULL);
  attribute->name = strdup(name);
  attribute->type = type;
  attribute->index = index;
  attribute->accessor = accessor;
}

void vulkan_attribute_deinit(vulkan_attribute *attribute) {
  free(attribute->name);
  attribute->name = 0;
  attribute->accessor = NULL;
}

void vulkan_mesh_primitive_init(vulkan_mesh_primitive *primitive, VkPrimitiveTopology topology,
                                size_t attributeCount) {
  primitive->topology = topology;
  primitive->indices = NULL;
  primitive->attributes = alloc_struct_array(vulkan_attribute, attributeCount);
  init_struct_array(primitive->attributes);
}

void vulkan_mesh_primitive_deinit(vulkan_mesh_primitive *primitive) {
  dealloc_struct(primitive->attributes);
}

void vulkan_mesh_init(vulkan_mesh *mesh, size_t primitiveCount) {
  mesh->primitives = alloc_struct_array(vulkan_mesh_primitive, primitiveCount);
  init_struct_array(mesh->primitives);
}

void vulkan_mesh_deinit(vulkan_mesh *mesh) { dealloc_struct(mesh->primitives); }

void vulkan_node_init(vulkan_node *scene, vulkan_mesh mesh) {
  scene->mesh = mesh;
  glm_mat4_identity(scene->transform);
}

void vulkan_node_deinit(vulkan_node *self) { vulkan_mesh_deinit(&self->mesh); }

void vulkan_scene_init(vulkan_scene *scene, size_t nodesCount, size_t bufferViewsCount,
                       size_t accessorsCount) {
  scene->nodes = alloc_struct_array(vulkan_node, nodesCount);
  init_struct_array(scene->nodes);
  scene->bufferViews = alloc_struct_array(vulkan_buffer_view, bufferViewsCount);
  init_struct_array(scene->bufferViews);
  scene->accessors = alloc_struct_array(vulkan_accessor, accessorsCount);
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
  for (size_t i = 0; i < count_struct_array(self->bufferViews); i++) {
    vulkan_buffer_view *bufferView = &self->bufferViews[i];
    log_debug("bufferView: '%s' %d %d %d\n", bufferView->name, bufferView->offset, bufferView->size,
              bufferView->stride);
  }
  for (size_t i = 0; i < count_struct_array(self->accessors); i++) {
    vulkan_accessor *accessor = &self->accessors[i];
    log_debug("accessor: '%s' %d %d %d '%s'\n", accessor->name, accessor->offset, accessor->count,
              accessor->stride, accessor->bufferView->name);
  }
  for (size_t i = 0; i < count_struct_array(self->nodes); i++) {
    log_debug("node:\n");
    vulkan_node *node = &self->nodes[i];
    glm_mat4_print(node->transform, stderr);
    vulkan_mesh *mesh = &node->mesh;
    for (size_t j = 0; j < count_struct_array(mesh->primitives); j++) {
      vulkan_mesh_primitive *primitive = &mesh->primitives[j];
      log_debug("  primitive %d: %s\n", j, VkPrimitiveTopology_debug_str(primitive->topology));
      if (primitive->indices != NULL) {
        vulkan_accessor *indices = primitive->indices;
        log_debug("    indices: '%s' %d %d %d '%s'\n", indices->name, indices->offset,
                  indices->count, indices->stride, indices->bufferView->name);
      }
      for (size_t k = 0; k < count_struct_array(primitive->attributes); k++) {
        vulkan_attribute *attribute = &primitive->attributes[k];
        log_debug("    attribute: '%s' %s %d\n", attribute->name,
                  vulkan_attribute_type_debug_str(attribute->type), attribute->index);
        vulkan_accessor *accessor = attribute->accessor;
        assert(accessor != NULL);
        if (accessor != NULL) {
          log_debug("      accessor: '%s' %d %d %d '%s'\n", accessor->name, accessor->offset,
                    accessor->count, accessor->stride, accessor->bufferView->name);
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

vulkan_attribute_type cgltf_attribute_type_vulkan_attribute_type(cgltf_attribute_type type) {
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

vulkan_buffer_view *find_buffer_view(vulkan_buffer_view *bufferViews,
                                     cgltf_buffer_view *cgltfBufferView) {
  for (size_t i = 0; i < count_struct_array(bufferViews); i++) {
    vulkan_buffer_view *bufferView = &bufferViews[i];
    if (bufferView->offset == cgltfBufferView->offset &&
        bufferView->size == cgltfBufferView->size &&
        bufferView->stride == cgltfBufferView->stride) {
      return bufferView;
    }
  }
  assert(0);
  return NULL;
}

vulkan_accessor *find_accessor(vulkan_accessor *accessors, vulkan_buffer_view *bufferViews,
                               cgltf_accessor *cgltfAccessor) {
  for (size_t i = 0; i < count_struct_array(accessors); i++) {
    vulkan_accessor *accessor = &accessors[i];
    vulkan_buffer_view *bufferView = find_buffer_view(bufferViews, cgltfAccessor->buffer_view);
    if (accessor->offset == cgltfAccessor->offset && accessor->count == cgltfAccessor->count &&
        accessor->stride == cgltfAccessor->stride &&
        strcmp(accessor->bufferView->name, bufferView->name) == 0) {
      return accessor;
    }
  }
  assert(0);
  return NULL;
}

vulkan_attribute parse_attribute_accessor(cgltf_attribute *cgltfAttribute,
                                          vulkan_buffer_view *bufferViews,
                                          vulkan_accessor *accessors) {
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
  vulkan_buffer_view *bufferView = find_buffer_view(bufferViews, cgltfBufferView);
  vulkan_attribute_type type = cgltf_attribute_type_vulkan_attribute_type(cgltfAttribute->type);
  vulkan_accessor *accessor = find_accessor(accessors, bufferViews, cgltfAccessor);
  vulkan_attribute attribute;
  vulkan_attribute_init(&attribute, cgltfAttribute->name, type, 0, accessor);
  return attribute;
}

vulkan_node parse_cgltf_node(cgltf_node *cgltfNode, vulkan_geometry_buffer *geometryBuffer,
                             vulkan_buffer_view *bufferViews, vulkan_accessor *accessors) {
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
    vulkan_accessor *indices = find_accessor(accessors, bufferViews, cgltfIndices);
    vulkan_mesh_primitive meshPrimitive;
    vulkan_mesh_primitive_init(&meshPrimitive, topology, cgltfPrimitive->attributes_count);
    meshPrimitive.indices = indices;

    for (size_t attributeIdx = 0; attributeIdx < cgltfPrimitive->attributes_count; attributeIdx++) {
      cgltf_attribute *cgltfAttribute = &cgltfPrimitive->attributes[attributeIdx];
      vulkan_attribute attribute = parse_attribute_accessor(cgltfAttribute, bufferViews, accessors);
      meshPrimitive.attributes[attributeIdx] = attribute;
    }
    // cgltf_material* material;
    mesh.primitives[primitiveIdx] = meshPrimitive;
  }
  vulkan_node node;
  vulkan_node_init(&node, mesh);
  cgltf_node_transform_local(cgltfNode, (float *)node.transform);
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
    vulkan_buffer_view bufferView;
    vulkan_buffer_view_init(&bufferView, name, cgltfBufferView->offset, cgltfBufferView->size,
                            cgltfBufferView->stride, scene->geometryBuffer);
    scene->bufferViews[i] = bufferView;
  }
  // parse accessors
  for (size_t i = 0; i < accessorsCount; i++) {
    sprintf(name, "accessor%d", (int)i);
    cgltf_accessor *cgltfAccessor = &data->accessors[i];
    vulkan_buffer_view *bufferView =
        find_buffer_view(scene->bufferViews, cgltfAccessor->buffer_view);
    vulkan_accessor accessor;
    vulkan_accessor_init(&accessor, name, cgltfAccessor->offset, cgltfAccessor->count,
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
