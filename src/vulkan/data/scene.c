#include "scene.h"

/* glTF parsing */

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

vulkan_material_data *parse_cgltf_material(vulkan_scene_data *sceneData,
                                           cgltf_material *cgltfMaterial) {
  if (cgltfMaterial == NULL) {
    return sceneData->materials;
  }
  vulkan_material_data material = {0};
  vulkan_material_data_init(&material);
  log_debug("cgltfMaterial->has_pbr_metallic_roughness=%d",
            cgltfMaterial->has_pbr_metallic_roughness);
  log_debug("cgltfMaterial->has_pbr_specular_glossiness=%d",
            cgltfMaterial->has_pbr_specular_glossiness);
  log_debug("cgltfMaterial->has_clearcoat=%d", cgltfMaterial->has_clearcoat);
  log_debug("cgltfMaterial->has_transmission=%d", cgltfMaterial->has_transmission);
  log_debug("cgltfMaterial->has_volume=%d", cgltfMaterial->has_volume);
  log_debug("cgltfMaterial->has_ior=%d", cgltfMaterial->has_ior);
  log_debug("cgltfMaterial->has_specular=%d", cgltfMaterial->has_specular);
  log_debug("cgltfMaterial->has_sheen=%d", cgltfMaterial->has_sheen);
  verify(cgltfMaterial->has_pbr_metallic_roughness); // TODO: Support specular-glossiness ext?
  if (cgltfMaterial->has_pbr_metallic_roughness) {
    material.baseColorFactor[0] = cgltfMaterial->pbr_metallic_roughness.base_color_factor[0];
    material.baseColorFactor[1] = cgltfMaterial->pbr_metallic_roughness.base_color_factor[1];
    material.baseColorFactor[2] = cgltfMaterial->pbr_metallic_roughness.base_color_factor[2];
    material.baseColorFactor[3] = cgltfMaterial->pbr_metallic_roughness.base_color_factor[3];
    // HIRO material texture
  }
  material.hash = vulkan_material_data_calculate_key(&material);

  vulkan_material_data *existingMaterial =
      vulkan_scene_data_get_material_by_key(sceneData, NULL, material.hash);
  if (existingMaterial != NULL) {
    return existingMaterial;
  }

  log_debug("adding new material");
  vulkan_material_data *newMaterial = core_alloc(sizeof(vulkan_material_data));
  vulkan_material_data_init(newMaterial);
  core_memcpy(newMaterial, &material, sizeof(material));
  DL_APPEND(sceneData->materials, newMaterial);
  return newMaterial;
}

vulkan_primitive_data *parse_cgltf_primitive(vulkan_scene_data *sceneData,
                                             cgltf_primitive *cgltfPrimitive) {
  // Check if glTF uses only supported capabilities.
  assert(cgltfPrimitive->has_draco_mesh_compression == false);
  assert(cgltfPrimitive->targets_count == 0);
  assert(cgltfPrimitive->mappings_count == 0);
  assert(cgltfPrimitive->indices != NULL);

  VkPrimitiveTopology topology = cgltf_primitive_type_vulkan_topology(cgltfPrimitive->type);
  cgltf_accessor *cgltfIndices = cgltfPrimitive->indices;
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

  vulkan_primitive_data primitive;
  vulkan_primitive_data_init(&primitive, sceneData);

  vulkan_material_data *material = parse_cgltf_material(sceneData, cgltfPrimitive->material);
  primitive.material = material;

  primitive.topology = topology;
  primitive.vertexCount = vertexCount;

  // read indices
  utarray_resize(primitive.indices, indexCount);
  for (size_t i = 0; i < utarray_len(primitive.indices); i++) {
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
    for (size_t attributeIdx = 0; attributeIdx < cgltfPrimitive->attributes_count; attributeIdx++) {
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

  primitive.hash = vulkan_primitive_data_calculate_key(&primitive);

  vulkan_primitive_data *existingPrimitive =
      vulkan_scene_data_get_primitive_by_key(sceneData, NULL, primitive.hash);
  if (existingPrimitive != NULL) {
    return existingPrimitive;
  }

  log_debug("adding new primitive");
  vulkan_primitive_data *newPrimitive = core_alloc(sizeof(vulkan_primitive_data));
  vulkan_primitive_data_init(newPrimitive, sceneData);
  core_memcpy(newPrimitive, &primitive, sizeof(primitive));
  DL_APPEND(sceneData->primitives, newPrimitive);
  return newPrimitive;
}

vulkan_mesh_data parse_cgltf_mesh(vulkan_scene_data *sceneData, cgltf_mesh *cgltfMesh) {
  vulkan_mesh_data mesh;
  vulkan_mesh_data_init(&mesh, sceneData);

  for (size_t primitiveIdx = 0; primitiveIdx < cgltfMesh->primitives_count; primitiveIdx++) {
    vulkan_primitive_data *primitive =
        parse_cgltf_primitive(sceneData, &cgltfMesh->primitives[primitiveIdx]);
    utarray_push_back(mesh.primitives, &primitive);
  }

  mesh.hash = vulkan_mesh_data_calculate_key(&mesh);

  return mesh;
}

vulkan_node_data parse_cgltf_node(vulkan_scene_data *sceneData, cgltf_node *cgltfNode) {
  assert(cgltfNode->extras.start_offset == 0 && cgltfNode->extras.end_offset == 0);
  assert(cgltfNode->camera == NULL);
  assert(cgltfNode->light == NULL);
  assert(cgltfNode->weights_count == 0);
  assert(cgltfNode->children_count == 0);
  cgltf_mesh *cgltfMesh = cgltfNode->mesh;
  assert(cgltfMesh != NULL);
  assert(cgltfMesh->extras.start_offset == 0 && cgltfMesh->extras.end_offset == 0);
  assert(cgltfMesh->target_names_count == 0);

  vulkan_node_data node;
  vulkan_node_data_init(&node, sceneData);

  node.mesh = parse_cgltf_mesh(sceneData, cgltfMesh);
  // TODO: Animation, will probably require cgltf_node_transform_local().
  cgltf_node_transform_world(cgltfNode, (float *)node.transform);
  node.hash = vulkan_node_data_calculate_key(&node);

  return node;
}

vulkan_scene_data *parse_cgltf_scene(UT_string *name, UT_string *path) {
  cgltf_options options = {0};
  cgltf_data *cgltfData = NULL;
  cgltf_result result = cgltf_result_success;
  result = cgltf_parse_file(&options, utstring_body(path), &cgltfData);
  if (result != cgltf_result_success) {
    panic("failed to parse gltf file");
  }
  result = cgltf_load_buffers(&options, cgltfData, utstring_body(path));
  if (result != cgltf_result_success) {
    panic("failed to load gltf buffers");
  }

  assert(cgltfData->scenes_count == 1); // We support only one scene per glTF file.
  cgltf_scene *cgltfScene = &cgltfData->scene[0];

  vulkan_scene_data *sceneData = vulkan_scene_data_create(name);

  // parse materials
  for (size_t materialIdx = 0; materialIdx < cgltfData->materials_count; materialIdx++) {
    parse_cgltf_material(sceneData, &cgltfData->materials[materialIdx]);
  }

  // parse nodes
  for (size_t nodeIdx = 0; nodeIdx < cgltfScene->nodes_count; nodeIdx++) {
    vulkan_node_data node = parse_cgltf_node(sceneData, cgltfScene->nodes[nodeIdx]);
    utarray_push_back(sceneData->nodes, &node);
  }

  // parse cameras
  vulkan_camera_data defaultCamera;
  vulkan_camera_data_init(&defaultCamera);
  utarray_push_back(sceneData->cameras, &defaultCamera);
  // TODO: cameras from gltf file

  cgltf_free(cgltfData);
  return sceneData;
}

/* scene data */

data_key vulkan_scene_data_calculate_key(vulkan_scene_data *sceneData) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, utstring_body(sceneData->name), utstring_len(sceneData->name))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState);
  return (data_key){value};
}

void vulkan_scene_data_serialize(vulkan_scene_data *sceneData, data_asset_db *assetDb) {
  UT_array *cameraKeys = NULL;
  utarray_alloc(cameraKeys, sizeof(data_key));
  vulkan_camera_data *camera = NULL;
  while ((camera = (utarray_next(sceneData->cameras, camera)))) {
    vulkan_camera_data_serialize(camera, assetDb);
    utarray_push_back(cameraKeys, &camera->hash);
  }
  data_asset_db_insert_scene_cameras_key_array(assetDb, sceneData->hash,
                                               data_key_array_temp(cameraKeys));
  utarray_free(cameraKeys);

  UT_array *nodeKeys = NULL;
  utarray_alloc(nodeKeys, sizeof(data_key));
  vulkan_node_data *node = NULL;
  while ((node = (utarray_next(sceneData->nodes, node)))) {
    // vulkan_node_data_debug_print(node);
    vulkan_node_data_serialize(node, assetDb);
    utarray_push_back(nodeKeys, &node->hash);
  }
  data_asset_db_insert_scene_name_text(assetDb, sceneData->hash, (data_text){sceneData->name});
  data_asset_db_insert_scene_nodes_key_array(assetDb, sceneData->hash,
                                             data_key_array_temp(nodeKeys));
  utarray_free(nodeKeys);

  // TODO: Add materials to pipeline.
}

void vulkan_scene_data_deserialize(vulkan_scene_data *sceneData, data_asset_db *assetDb,
                                   data_key key) {
  sceneData->hash = key;

  data_key_array cameraKeyArray =
      data_asset_db_select_scene_cameras_key_array(assetDb, sceneData->hash);
  data_key_array nodeKeyArray =
      data_asset_db_select_scene_nodes_key_array(assetDb, sceneData->hash);

  data_key *cameraHash = NULL;
  while ((cameraHash = (utarray_next(cameraKeyArray.values, cameraHash)))) {
    vulkan_camera_data camera;
    vulkan_camera_data_init(&camera);
    vulkan_camera_data_deserialize(&camera, assetDb, *cameraHash);
    utarray_push_back(sceneData->cameras, &camera);
  }

  data_key *nodeKey = NULL;
  while ((nodeKey = (utarray_next(nodeKeyArray.values, nodeKey)))) {
    /* node data */
    vulkan_node_data node;
    vulkan_node_data_init(&node, sceneData);
    vulkan_node_data_deserialize(&node, assetDb, *nodeKey);
    utarray_push_back(sceneData->nodes, &node);
  }
  data_key_array_deinit(&nodeKeyArray);
}

vulkan_scene_data *vulkan_scene_data_create(UT_string *name) {
  vulkan_scene_data *sceneData = core_alloc(sizeof(vulkan_scene_data));
  utstring_new(sceneData->name);
  utstring_concat(sceneData->name, name);

  sceneData->materials = NULL;
  vulkan_material_data *defaultMaterial = core_alloc(sizeof(vulkan_material_data));
  vulkan_material_data_init(defaultMaterial);
  DL_APPEND(sceneData->materials, defaultMaterial);

  sceneData->primitives = NULL;

  utarray_alloc(sceneData->nodes, sizeof(vulkan_node_data));
  utarray_alloc(sceneData->cameras, sizeof(vulkan_camera_data));
  sceneData->dirty = true;
  sceneData->hash = vulkan_scene_data_calculate_key(sceneData);
  return sceneData;
}

void vulkan_scene_data_destroy(vulkan_scene_data *sceneData) {
  utstring_free(sceneData->name);

  vulkan_material_data *material, *tempMaterial;
  DL_FOREACH_SAFE(sceneData->materials, material, tempMaterial) {
    vulkan_material_data_deinit(material);
    core_free(material);
  }

  vulkan_primitive_data *primitive, *tempPrimitive;
  DL_FOREACH_SAFE(sceneData->primitives, primitive, tempPrimitive) {
    vulkan_primitive_data_deinit(primitive);
    core_free(primitive);
  }

  vulkan_node_data *node = NULL;
  while ((node = (utarray_next(sceneData->nodes, node)))) {
    vulkan_node_data_deinit(node);
  }
  utarray_free(sceneData->nodes);

  vulkan_camera_data *camera = NULL;
  while ((camera = (utarray_next(sceneData->cameras, camera)))) {
    vulkan_camera_data_deinit(camera);
  }
  utarray_free(sceneData->cameras);

  core_free(sceneData);
}

vulkan_material_data *vulkan_scene_data_get_material_by_key(vulkan_scene_data *sceneData,
                                                            data_asset_db *assetDb, data_key key) {
  vulkan_material_data *material = NULL;

  vulkan_material_data *existingMaterial = NULL;
  DL_FOREACH(sceneData->materials, existingMaterial) {
    if (existingMaterial->hash.value == key.value) {
      material = existingMaterial;
      break;
    }
  }

  if (material == NULL && assetDb != NULL) {
    vulkan_material_data *newMaterial = core_alloc(sizeof(vulkan_material_data));
    vulkan_material_data_init(newMaterial);
    vulkan_material_data_deserialize(newMaterial, assetDb, key);
    LL_PREPEND(sceneData->materials, newMaterial);
    material = sceneData->materials;
  }

  return material;
}

vulkan_primitive_data *vulkan_scene_data_get_primitive_by_key(vulkan_scene_data *sceneData,
                                                              data_asset_db *assetDb,
                                                              data_key key) {
  vulkan_primitive_data *primitive = NULL;

  vulkan_primitive_data *existingPrimitive = NULL;
  DL_FOREACH(sceneData->primitives, existingPrimitive) {
    if (existingPrimitive->hash.value == key.value) {
      primitive = existingPrimitive;
      break;
    }
  }

  if (primitive == NULL && assetDb != NULL) {
    vulkan_primitive_data *newPrimitive = core_alloc(sizeof(vulkan_primitive_data));
    vulkan_primitive_data_init(newPrimitive, sceneData);
    vulkan_primitive_data_deserialize(newPrimitive, assetDb, key);
    LL_PREPEND(sceneData->primitives, newPrimitive);
    primitive = sceneData->primitives;
  }

  return primitive;
}

vulkan_scene_data *vulkan_scene_data_create_with_gltf_file(UT_string *sceneName,
                                                           UT_string *gltfPath) {
  vulkan_scene_data *sceneData = parse_cgltf_scene(sceneName, gltfPath);
  return sceneData;
}

vulkan_scene_data *vulkan_scene_data_create_with_asset_db(data_asset_db *assetDb,
                                                          UT_string *sceneName) {
  vulkan_scene_data *sceneData = NULL;
  sceneData = vulkan_scene_data_create(sceneName);
  data_key sceneKey = vulkan_scene_data_calculate_key(sceneData);
  vulkan_scene_data_deserialize(sceneData, assetDb, sceneKey);
  return sceneData;
}

/* debug print */

void vulkan_scene_data_debug_print(vulkan_scene_data *sceneData) {
  log_debug("SCENE_DATA:\n");
  vulkan_node_data *node = NULL;
  while ((node = (utarray_next(sceneData->nodes, node)))) {
    vulkan_node_data_debug_print(node);
  }
}
