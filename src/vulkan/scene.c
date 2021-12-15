#include "scene.h"
#include "../codegen/scene.c"

vulkan_buffer_view vulkan_buffer_view_init(char *name, size_t offset,
                                           size_t size, size_t stride,
                                           vulkan_geometry_buffer *buffer) {
  vulkan_buffer_view result = {0};
  result.name = strdup(name);
  result.offset = offset;
  result.size = size;
  result.stride = stride;
  result.buffer = buffer;
  return result;
}

void vulkan_buffer_view_free(vulkan_buffer_view *self) {
  free(self->name);
  self->name = "FREE";
}

vulkan_buffer_view vulkan_buffer_view_copy(vulkan_buffer_view *self) {
  vulkan_buffer_view copy = *self;
  copy.name = strdup(self->name);
  return copy;
}

vulkan_accessor vulkan_accessor_init(char *name, size_t offset, size_t size,
                                     size_t stride,
                                     vulkan_buffer_view *bufferView) {
  vulkan_accessor result = {0};
  result.name = strdup(name);
  result.offset = offset;
  result.count = size;
  result.stride = stride;
  result.bufferView = bufferView;
  return result;
}
void vulkan_accessor_free(vulkan_accessor *self) {
  free(self->name);
  self->name = "FREE";
}

vulkan_accessor vulkan_accessor_copy(vulkan_accessor *self) {
  vulkan_accessor copy = *self;
  copy.name = strdup(self->name);
  return copy;
}

vulkan_attribute vulkan_attribute_init(char *name, vulkan_attribute_type type,
                                       size_t index,
                                       vulkan_accessor *accessor) {
  assert(name != NULL);
  vulkan_attribute result = {0};
  result.name = strdup(name);
  result.type = type;
  result.index = index;
  result.accessor = accessor;
  return result;
}

void vulkan_attribute_free(vulkan_attribute *self) {
  free(self->name);
  self->name = 0;
  self->accessor = NULL;
}

vulkan_attribute vulkan_attribute_copy(vulkan_attribute *self) {
  vulkan_attribute copy = {0};
  copy.name = strdup(self->name);
  copy.type = self->type;
  copy.index = self->index;
  copy.accessor = self->accessor;
  return copy;
}

vulkan_mesh_primitive vulkan_mesh_primitive_init(VkPrimitiveTopology topology) {
  vulkan_mesh_primitive result = {0};
  result.topology = topology;
  result.indices = NULL;
  result.attributes = vec_vulkan_attribute_init();
  return result;
}

void vulkan_mesh_primitive_free(vulkan_mesh_primitive *self) {
  vec_vulkan_attribute_free(&self->attributes);
}

vulkan_mesh_primitive vulkan_mesh_primitive_copy(vulkan_mesh_primitive *self) {
  vulkan_mesh_primitive copy = {0};
  copy.topology = self->topology;
  copy.indices = self->indices;
  copy.attributes = vec_vulkan_attribute_copy(&self->attributes);
  return copy;
}

void vulkan_mesh_primitive_add_attribute(vulkan_mesh_primitive *self,
                                         vulkan_attribute attribute) {
  vec_vulkan_attribute_push_back(&self->attributes, attribute);
}

vulkan_mesh vulkan_mesh_init() {
  vulkan_mesh result = {0};
  result.primitives = vec_vulkan_mesh_primitive_init();
  return result;
}

void vulkan_mesh_free(vulkan_mesh *self) {
  vec_vulkan_mesh_primitive_free(&self->primitives);
}

vulkan_mesh vulkan_mesh_copy(vulkan_mesh *self) {
  vulkan_mesh copy = {0};
  copy.primitives = vec_vulkan_mesh_primitive_copy(&self->primitives);
  return copy;
}

void vulkan_mesh_add_mesh_primitive(vulkan_mesh *self,
                                    vulkan_mesh_primitive meshPrimitive) {
  vec_vulkan_mesh_primitive_push_back(&self->primitives, meshPrimitive);
}

vulkan_node vulkan_node_init(vulkan_mesh mesh) {
  vulkan_node result = {0};
  result.mesh = mesh;
  glm_mat4_identity(result.transform);
  return result;
}

void vulkan_node_free(vulkan_node *self) { vulkan_mesh_free(&self->mesh); }

vulkan_node vulkan_node_copy(vulkan_node *self) {
  vulkan_node copy = {0};
  copy.mesh = vulkan_mesh_copy(&self->mesh);
  glm_mat4_copy(self->transform, copy.transform);
  return copy;
}

void vulkan_scene_init(vulkan_scene *scene) {
  scene->nodes = vec_vulkan_node_init();
  scene->bufferViews = vec_vulkan_buffer_view_init();
  scene->accessors = vec_vulkan_accessor_init();
  scene->accessors = vec_vulkan_accessor_init();
}

void vulkan_scene_deinit(vulkan_scene *self) {
  vec_vulkan_node_free(&self->nodes);
  vulkan_geometry_buffer_deinit(&self->geometryBuffer);
  vec_vulkan_buffer_view_free(&self->bufferViews);
  vec_vulkan_accessor_free(&self->accessors);
}

void vulkan_scene_add_node(vulkan_scene *self, vulkan_node node) {
  vec_vulkan_node_push_back(&self->nodes, node);
}

void vulkan_scene_debug_print(vulkan_scene *self) {
  log_debug("SCENE:\n");
  log_debug("geometryBuffer: %d\n", self->geometryBuffer.dataSize);
  for (size_t i = 0; i < self->bufferViews.size; i++) {
    vulkan_buffer_view *bufferView = &self->bufferViews.value[i];
    log_debug("bufferView: '%s' %d %d %d\n", bufferView->name,
              bufferView->offset, bufferView->size, bufferView->stride);
  }
  for (size_t i = 0; i < self->accessors.size; i++) {
    vulkan_accessor *accessor = &self->accessors.value[i];
    log_debug("accessor: '%s' %d %d %d '%s'\n", accessor->name, accessor->offset, accessor->count,
              accessor->stride, accessor->bufferView->name);
  }
  for (size_t i = 0; i < self->nodes.size; i++) {
    log_debug("node:\n");
    vulkan_node *node = &self->nodes.value[i];
    glm_mat4_print(node->transform, stderr);
    vulkan_mesh *mesh = &node->mesh;
    for (size_t j = 0; j < mesh->primitives.size; j++) {
      vulkan_mesh_primitive *primitive = &mesh->primitives.value[j];
      log_debug("  primitive %d: %s\n", j,
                VkPrimitiveTopology_debug_str(primitive->topology));
      if (primitive->indices != NULL) {
        vulkan_accessor *indices = primitive->indices;
        log_debug("    indices: '%s' %d %d %d '%s'\n", indices->name, indices->offset,
                  indices->count, indices->stride, indices->bufferView->name);
      }
      for (size_t k = 0; k < primitive->attributes.size; k++) {
        vulkan_attribute *attribute = &primitive->attributes.value[k];
        log_debug("    attribute: '%s' %s %d\n", attribute->name,
                  vulkan_attribute_type_debug_str(attribute->type),
                  attribute->index);
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

VkPrimitiveTopology
cgltf_primitive_type_vulkan_topology(cgltf_primitive_type type) {
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

vulkan_attribute_type
cgltf_attribute_type_vulkan_attribute_type(cgltf_attribute_type type) {
  switch (type) {
  case cgltf_attribute_type_position:
    return PositionAttribute;
  case cgltf_attribute_type_normal:
    return NormalAttribute;
  case cgltf_attribute_type_tangent:
    return TangentAttribute;
  case cgltf_attribute_type_texcoord:
    return TexcoordAttribute;
  case cgltf_attribute_type_color:
    return ColorAttribute;
  case cgltf_attribute_type_joints:
  case cgltf_attribute_type_weights:
  case cgltf_attribute_type_invalid:
  default:
    return UnknownAttribute;
  }
}

vulkan_buffer_view *find_buffer_view(vec_vulkan_buffer_view *bufferViews,
                                     cgltf_buffer_view *cgltfBufferView) {
  for (size_t i = 0; i < bufferViews->size; i++) {
    vulkan_buffer_view *bufferView = &bufferViews->value[i];
    if (bufferView->offset == cgltfBufferView->offset &&
        bufferView->size == cgltfBufferView->size &&
        bufferView->stride == cgltfBufferView->stride) {
      return bufferView;
    }
  }
  assert(0);
  return NULL;
}

vulkan_accessor *find_accessor(vec_vulkan_accessor *accessors,
                               vec_vulkan_buffer_view *bufferViews,
                               cgltf_accessor *cgltfAccessor) {
  for (size_t i = 0; i < accessors->size; i++) {
    vulkan_accessor *accessor = &accessors->value[i];
    vulkan_buffer_view *bufferView =
        find_buffer_view(bufferViews, cgltfAccessor->buffer_view);
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
                                          vec_vulkan_buffer_view *bufferViews,
                                          vec_vulkan_accessor *accessors) {
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
  vulkan_buffer_view *bufferView =
      find_buffer_view(bufferViews, cgltfBufferView);
  vulkan_attribute_type type =
      cgltf_attribute_type_vulkan_attribute_type(cgltfAttribute->type);
  vulkan_accessor *accessor =
      find_accessor(accessors, bufferViews, cgltfAccessor);
  vulkan_attribute attribute =
      vulkan_attribute_init(cgltfAttribute->name, type, 0, accessor);
  return attribute;
}

vulkan_node parse_cgltf_node(cgltf_node *cgltfNode,
                             vulkan_geometry_buffer *geometryBuffer,
                             vec_vulkan_buffer_view *bufferViews,
                             vec_vulkan_accessor *accessors) {
  assert(cgltfNode->extras.start_offset == 0 &&
         cgltfNode->extras.end_offset == 0);
  assert(cgltfNode->camera == NULL);
  assert(cgltfNode->light == NULL);
  assert(cgltfNode->weights_count == 0);
  assert(cgltfNode->children_count == 0);
  cgltf_mesh *cgltfMesh = cgltfNode->mesh;
  assert(cgltfMesh != NULL);
  assert(cgltfMesh->extras.start_offset == 0 &&
         cgltfMesh->extras.end_offset == 0);
  assert(cgltfMesh->target_names_count == 0);

  vulkan_mesh mesh = vulkan_mesh_init();

  for (size_t primitiveIdx = 0; primitiveIdx < cgltfMesh->primitives_count;
       primitiveIdx++) {
    cgltf_primitive *cgltfPrimitive = &cgltfMesh->primitives[primitiveIdx];
    assert(cgltfPrimitive->has_draco_mesh_compression == false);
    assert(cgltfPrimitive->targets_count == 0);
    assert(cgltfPrimitive->mappings_count == 0);
    cgltf_accessor *cgltfIndices = cgltfPrimitive->indices;
    assert(cgltfIndices != NULL);

    VkPrimitiveTopology topology =
        cgltf_primitive_type_vulkan_topology(cgltfPrimitive->type);
    vulkan_accessor *indices =
        find_accessor(accessors, bufferViews, cgltfIndices);
    vulkan_mesh_primitive meshPrimitive = vulkan_mesh_primitive_init(topology);
    meshPrimitive.indices = indices;

    for (size_t attributeIdx = 0;
         attributeIdx < cgltfPrimitive->attributes_count; attributeIdx++) {
      cgltf_attribute *cgltfAttribute =
          &cgltfPrimitive->attributes[attributeIdx];
      vulkan_attribute attribute =
          parse_attribute_accessor(cgltfAttribute, bufferViews, accessors);
      vulkan_mesh_primitive_add_attribute(&meshPrimitive, attribute);
    }
    // cgltf_material* material;
    vulkan_mesh_add_mesh_primitive(&mesh, meshPrimitive);
  }
  vulkan_node node = vulkan_node_init(mesh);
  cgltf_node_transform_local(cgltfNode, (float *)node.transform);
  return node;
}

void parse_gltf_file(vulkan_scene *scene, platform_path gltfPath) {
  vulkan_scene_init(scene);
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
  // parse geometry buffer
  assert(data->buffers_count == 1);
  cgltf_buffer *cgltfBuffer = &data->buffers[0];
  vulkan_geometry_buffer_init(&scene->geometryBuffer, cgltfBuffer->uri, cgltfBuffer->data,
                              cgltfBuffer->size);
  // parse buffer views
  char *name =
      malloc(sizeof(char) * (255 + (int)log10(data->buffer_views_count)));
  for (size_t i = 0; i < data->buffer_views_count; i++) {
    sprintf(name, "bufferView%d", (int)i);
    cgltf_buffer_view *cgltfBufferView = &data->buffer_views[i];
    assert(cgltfBufferView->has_meshopt_compression == false);
    vulkan_buffer_view bufferView =
        vulkan_buffer_view_init(name, cgltfBufferView->offset, cgltfBufferView->size,
                                cgltfBufferView->stride, &scene->geometryBuffer);
    vec_vulkan_buffer_view_push_back(&scene->bufferViews, bufferView);
  }
  // parse accessors
  for (size_t i = 0; i < data->accessors_count; i++) {
    sprintf(name, "accessor%d", (int)i);
    cgltf_accessor *cgltfAccessor = &data->accessors[i];
    vulkan_buffer_view *bufferView =
        find_buffer_view(&scene->bufferViews, cgltfAccessor->buffer_view);
    vulkan_accessor accessor = vulkan_accessor_init(
        name, cgltfAccessor->offset, cgltfAccessor->count, cgltfAccessor->stride, bufferView);
    vec_vulkan_accessor_push_back(&scene->accessors, accessor);
  }
  free(name);
  // parse scene
  assert(data->scenes_count == 1);
  cgltf_scene *cgltfScene = &data->scene[0];
  for (size_t nodeIdx = 0; nodeIdx < cgltfScene->nodes_count; nodeIdx++) {
    vulkan_node node = parse_cgltf_node(cgltfScene->nodes[nodeIdx], &scene->geometryBuffer,
                                        &scene->bufferViews, &scene->accessors);
    vulkan_scene_add_node(scene, node);
  }
  cgltf_free(data);
}
