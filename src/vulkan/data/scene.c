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

vulkan_data_image *parse_cgltf_image(vulkan_data_scene *sceneData, cgltf_image *cgltfImage) {
  if (cgltfImage == NULL) {
    return sceneData->images;
  }
  vulkan_data_image image = {0};
  vulkan_data_image_init(&image);

  UT_string *imagePath = get_path_dirname(sceneData->path);
  append_to_path(imagePath, cgltfImage->uri);
  log_debug("%s", utstring_body(imagePath));
  int w, h, c;
  stbi_uc *pixels = stbi_load(utstring_body(imagePath), &w, &h, &c, STBI_rgb_alpha);
  image.width = w;
  image.height = h;
  image.depth = 1;
  image.channels = c;
  size_t imageSize = image.width * image.height * image.depth * image.channels;
  utarray_resize(image.data, imageSize);
  core_memcpy(utarray_front(image.data), pixels, imageSize);
  core_free(pixels);

  image.hash = vulkan_data_image_calculate_key(&image);

  vulkan_data_image *existingImage =
      vulkan_data_scene_get_image_by_key(sceneData, NULL, image.hash);
  if (existingImage != NULL) {
    return existingImage;
  }

  log_debug("adding new image");
  vulkan_data_image *newImage = core_alloc(sizeof(vulkan_data_image));
  vulkan_data_image_init(newImage);
  core_memcpy(newImage, &image, sizeof(image));
  DL_APPEND(sceneData->images, newImage);
  return newImage;
}

vulkan_data_sampler *parse_cgltf_sampler(vulkan_data_scene *sceneData,
                                         cgltf_sampler *cgltfSampler) {
  if (cgltfSampler == NULL) {
    return sceneData->samplers;
  }
  vulkan_data_sampler sampler = {0};
  vulkan_data_sampler_init(&sampler);

  // sampler.image = parse_cgltf_image(sceneData, cgltfSampler->image);

  sampler.hash = vulkan_data_sampler_calculate_key(&sampler);

  vulkan_data_sampler *existingSampler =
      vulkan_data_scene_get_sampler_by_key(sceneData, NULL, sampler.hash);
  if (existingSampler != NULL) {
    return existingSampler;
  }

  log_debug("adding new sampler");
  vulkan_data_sampler *newSampler = core_alloc(sizeof(vulkan_data_sampler));
  vulkan_data_sampler_init(newSampler);
  core_memcpy(newSampler, &sampler, sizeof(sampler));
  DL_APPEND(sceneData->samplers, newSampler);
  return newSampler;
}

vulkan_data_texture *parse_cgltf_texture(vulkan_data_scene *sceneData,
                                         cgltf_texture *cgltfTexture) {
  if (cgltfTexture == NULL) {
    return sceneData->textures;
  }
  vulkan_data_texture texture = {0};
  vulkan_data_texture_init(&texture, sceneData);

  texture.image = parse_cgltf_image(sceneData, cgltfTexture->image);
  texture.sampler = parse_cgltf_sampler(sceneData, cgltfTexture->sampler);

  texture.hash = vulkan_data_texture_calculate_key(&texture);

  vulkan_data_texture *existingTexture =
      vulkan_data_scene_get_texture_by_key(sceneData, NULL, texture.hash);
  if (existingTexture != NULL) {
    return existingTexture;
  }

  log_debug("adding new texture");
  vulkan_data_texture *newTexture = core_alloc(sizeof(vulkan_data_texture));
  vulkan_data_texture_init(newTexture, sceneData);
  core_memcpy(newTexture, &texture, sizeof(texture));
  DL_APPEND(sceneData->textures, newTexture);
  return newTexture;
}

vulkan_data_material *parse_cgltf_material(vulkan_data_scene *sceneData,
                                           cgltf_material *cgltfMaterial) {
  if (cgltfMaterial == NULL) {
    return sceneData->materials;
  }
  vulkan_data_material material = {0};
  vulkan_data_material_init(&material, sceneData);
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
    material.metallicFactor = cgltfMaterial->pbr_metallic_roughness.metallic_factor;
    material.roughnessFactor = cgltfMaterial->pbr_metallic_roughness.roughness_factor;

    material.baseColorTexture = parse_cgltf_texture(
        sceneData, cgltfMaterial->pbr_metallic_roughness.base_color_texture.texture);
    material.metallicRoughnessTexture = parse_cgltf_texture(
        sceneData, cgltfMaterial->pbr_metallic_roughness.metallic_roughness_texture.texture);
  }
  material.hash = vulkan_data_material_calculate_key(&material);

  vulkan_data_material *existingMaterial =
      vulkan_data_scene_get_material_by_key(sceneData, NULL, material.hash);
  if (existingMaterial != NULL) {
    return existingMaterial;
  }

  log_debug("adding new material");
  vulkan_data_material *newMaterial = core_alloc(sizeof(vulkan_data_material));
  vulkan_data_material_init(newMaterial, sceneData);
  core_memcpy(newMaterial, &material, sizeof(material));
  DL_APPEND(sceneData->materials, newMaterial);
  return newMaterial;
}

vulkan_data_primitive *parse_cgltf_primitive(vulkan_data_scene *sceneData,
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

  vulkan_data_primitive *primitive = core_alloc(sizeof(vulkan_data_primitive));
  vulkan_data_primitive_init(primitive, sceneData);

  vulkan_data_material *material = parse_cgltf_material(sceneData, cgltfPrimitive->material);
  primitive->material = material;

  primitive->topology = topology;
  primitive->vertexCount = vertexCount;

  // read indices
  utarray_resize(primitive->indices, indexCount);
  for (size_t i = 0; i < utarray_len(primitive->indices); i++) {
    size_t indexValue = cgltf_accessor_read_index(cgltfIndices, i);
    void *outValue = utarray_eltptr(primitive->indices, i);
    *(uint32_t *)outValue = indexValue;
  }

  // read vertex attributes
  if ((vertexAttributes & PositionAttribute) != 0) {
    utarray_resize(primitive->positions, primitive->vertexCount);
  }
  if ((vertexAttributes & NormalAttribute) != 0) {
    utarray_resize(primitive->normals, primitive->vertexCount);
  }
  if ((vertexAttributes & ColorAttribute) != 0) {
    utarray_resize(primitive->colors, primitive->vertexCount);
  }
  if ((vertexAttributes & TexCoordAttribute) != 0) {
    utarray_resize(primitive->texCoords, primitive->vertexCount);
  }
  for (size_t i = 0; i < primitive->vertexCount; i++) {
    for (size_t attributeIdx = 0; attributeIdx < cgltfPrimitive->attributes_count; attributeIdx++) {
      cgltf_attribute *cgltfAttribute = &cgltfPrimitive->attributes[attributeIdx];
      vulkan_attribute_type type = cgltf_to_vulkan_attribute_type(cgltfAttribute->type);
      if (type == PositionAttribute) {
        verify(cgltf_accessor_read_float(cgltfAttribute->data, i,
                                         (float *)utarray_eltptr(primitive->positions, i), 3));
      }
      if (type == NormalAttribute) {
        verify(cgltf_accessor_read_float(cgltfAttribute->data, i,
                                         (float *)utarray_eltptr(primitive->normals, i), 3));
      }
      if (type == ColorAttribute) {
        verify(cgltf_accessor_read_float(cgltfAttribute->data, i,
                                         (float *)utarray_eltptr(primitive->colors, i), 3));
      }
      if (type == TexCoordAttribute) {
        verify(cgltf_accessor_read_float(cgltfAttribute->data, i,
                                         (float *)utarray_eltptr(primitive->texCoords, i), 2));
      }
    }
  }

  primitive->hash = vulkan_data_primitive_calculate_key(primitive);

  vulkan_data_primitive *existingPrimitive =
      vulkan_data_scene_get_primitive_by_key(sceneData, NULL, primitive->hash);
  if (existingPrimitive != NULL) {
    core_free(primitive);
    return existingPrimitive;
  }

  log_debug("adding new primitive");
  DL_APPEND(sceneData->primitives, primitive);
  return primitive;
}

vulkan_data_mesh parse_cgltf_mesh(vulkan_data_scene *sceneData, cgltf_mesh *cgltfMesh) {
  vulkan_data_mesh mesh;
  vulkan_data_mesh_init(&mesh, sceneData);

  for (size_t primitiveIdx = 0; primitiveIdx < cgltfMesh->primitives_count; primitiveIdx++) {
    vulkan_data_primitive *primitive =
        parse_cgltf_primitive(sceneData, &cgltfMesh->primitives[primitiveIdx]);
    utarray_push_back(mesh.primitives, &primitive);
  }

  mesh.hash = vulkan_data_mesh_calculate_key(&mesh);

  return mesh;
}

vulkan_data_object parse_cgltf_object(vulkan_data_scene *sceneData, cgltf_object *cgltfNode) {
  assert(cgltfNode->extras.start_offset == 0 && cgltfNode->extras.end_offset == 0);
  assert(cgltfNode->camera == NULL);
  assert(cgltfNode->light == NULL);
  assert(cgltfNode->weights_count == 0);
  assert(cgltfNode->children_count == 0);
  cgltf_mesh *cgltfMesh = cgltfNode->mesh;
  assert(cgltfMesh != NULL);
  assert(cgltfMesh->extras.start_offset == 0 && cgltfMesh->extras.end_offset == 0);
  assert(cgltfMesh->target_names_count == 0);

  vulkan_data_object object;
  vulkan_data_object_init(&object, sceneData);

  object.mesh = parse_cgltf_mesh(sceneData, cgltfMesh);
  // TODO: Animation, will probably require cgltf_object_transform_local().
  cgltf_object_transform_world(cgltfNode, (float *)object.transform);
  object.hash = vulkan_data_object_calculate_key(&object);

  return object;
}

vulkan_data_scene *parse_cgltf_scene(UT_string *name, UT_string *path) {
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

  vulkan_data_scene *sceneData = vulkan_data_scene_create(name);
  utstring_concat(sceneData->path, path);

  // parse objects
  for (size_t objectIdx = 0; objectIdx < cgltfScene->objects_count; objectIdx++) {
    vulkan_data_object object = parse_cgltf_object(sceneData, cgltfScene->objects[objectIdx]);
    utarray_push_back(sceneData->objects, &object);
  }

  // parse cameras
  vulkan_data_camera defaultCamera;
  vulkan_data_camera_init(&defaultCamera);
  utarray_push_back(sceneData->cameras, &defaultCamera);
  // TODO: cameras from gltf file

  cgltf_free(cgltfData);
  return sceneData;
}

/* scene data */

data_key vulkan_data_scene_calculate_key(vulkan_data_scene *sceneData) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, utstring_body(sceneData->name), utstring_len(sceneData->name))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState);
  return (data_key){value};
}

void vulkan_data_scene_serialize(vulkan_data_scene *sceneData, data_asset_db *assetDb) {
  UT_array *cameraKeys = NULL;
  utarray_alloc(cameraKeys, sizeof(data_key));
  vulkan_data_camera *camera = NULL;
  while ((camera = (utarray_next(sceneData->cameras, camera)))) {
    vulkan_data_camera_serialize(camera, assetDb);
    utarray_push_back(cameraKeys, &camera->hash);
  }
  data_asset_db_insert_scene_cameras_key_array(assetDb, sceneData->hash,
                                               data_key_array_temp(cameraKeys));
  utarray_free(cameraKeys);

  UT_array *objectKeys = NULL;
  utarray_alloc(objectKeys, sizeof(data_key));
  vulkan_data_object *object = NULL;
  while ((object = (utarray_next(sceneData->objects, object)))) {
    // vulkan_data_object_debug_print(object);
    vulkan_data_object_serialize(object, assetDb);
    utarray_push_back(objectKeys, &object->hash);
  }
  data_asset_db_insert_scene_name_text(assetDb, sceneData->hash, (data_text){sceneData->name});
  data_asset_db_insert_scene_objects_key_array(assetDb, sceneData->hash,
                                               data_key_array_temp(objectKeys));
  utarray_free(objectKeys);
}

void vulkan_data_scene_deserialize(vulkan_data_scene *sceneData, data_asset_db *assetDb,
                                   data_key key) {
  sceneData->hash = key;

  data_key_array cameraKeyArray =
      data_asset_db_select_scene_cameras_key_array(assetDb, sceneData->hash);
  data_key_array objectKeyArray =
      data_asset_db_select_scene_objects_key_array(assetDb, sceneData->hash);

  data_key *cameraHash = NULL;
  while ((cameraHash = (utarray_next(cameraKeyArray.values, cameraHash)))) {
    vulkan_data_camera camera;
    vulkan_data_camera_init(&camera);
    vulkan_data_camera_deserialize(&camera, assetDb, *cameraHash);
    utarray_push_back(sceneData->cameras, &camera);
  }

  data_key *objectKey = NULL;
  while ((objectKey = (utarray_next(objectKeyArray.values, objectKey)))) {
    /* object data */
    vulkan_data_object object;
    vulkan_data_object_init(&object, sceneData);
    vulkan_data_object_deserialize(&object, assetDb, *objectKey);
    utarray_push_back(sceneData->objects, &object);
  }
  data_key_array_deinit(&objectKeyArray);
}

vulkan_data_scene *vulkan_data_scene_create(UT_string *name) {
  vulkan_data_scene *sceneData = core_alloc(sizeof(vulkan_data_scene));
  utstring_new(sceneData->name);
  utstring_concat(sceneData->name, name);

  utstring_new(sceneData->path);

  sceneData->images = NULL;
  vulkan_data_image *defaultImage = core_alloc(sizeof(vulkan_data_image));
  vulkan_data_image_init(defaultImage);
  DL_APPEND(sceneData->images, defaultImage);

  sceneData->samplers = NULL;
  vulkan_data_sampler *defaultSampler = core_alloc(sizeof(vulkan_data_sampler));
  vulkan_data_sampler_init(defaultSampler);
  DL_APPEND(sceneData->samplers, defaultSampler);

  sceneData->textures = NULL;
  vulkan_data_texture *defaultTexture = core_alloc(sizeof(vulkan_data_texture));
  vulkan_data_texture_init(defaultTexture, sceneData);
  defaultTexture->image = defaultImage;
  defaultTexture->sampler = defaultSampler;
  defaultTexture->hash = vulkan_data_texture_calculate_key(defaultTexture);
  DL_APPEND(sceneData->textures, defaultTexture);

  sceneData->materials = NULL;
  vulkan_data_material *defaultMaterial = core_alloc(sizeof(vulkan_data_material));
  vulkan_data_material_init(defaultMaterial, sceneData);
  defaultMaterial->baseColorTexture = defaultTexture;
  defaultMaterial->metallicRoughnessTexture = defaultTexture;
  defaultMaterial->hash = vulkan_data_material_calculate_key(defaultMaterial);
  DL_APPEND(sceneData->materials, defaultMaterial);

  sceneData->primitives = NULL;

  utarray_alloc(sceneData->objects, sizeof(vulkan_data_object));
  utarray_alloc(sceneData->cameras, sizeof(vulkan_data_camera));
  sceneData->dirty = true;
  sceneData->hash = vulkan_data_scene_calculate_key(sceneData);
  return sceneData;
}

void vulkan_data_scene_destroy(vulkan_data_scene *sceneData) {
  utstring_free(sceneData->name);
  utstring_free(sceneData->path);

  vulkan_data_image *image, *tempImage;
  DL_FOREACH_SAFE(sceneData->images, image, tempImage) {
    vulkan_data_image_deinit(image);
    core_free(image);
  }

  vulkan_data_sampler *sampler, *tempSampler;
  DL_FOREACH_SAFE(sceneData->samplers, sampler, tempSampler) {
    vulkan_data_sampler_deinit(sampler);
    core_free(sampler);
  }

  vulkan_data_texture *texture, *tempTexture;
  DL_FOREACH_SAFE(sceneData->textures, texture, tempTexture) {
    vulkan_data_texture_deinit(texture);
    core_free(texture);
  }

  vulkan_data_material *material, *tempMaterial;
  DL_FOREACH_SAFE(sceneData->materials, material, tempMaterial) {
    vulkan_data_material_deinit(material);
    core_free(material);
  }

  vulkan_data_primitive *primitive, *tempPrimitive;
  DL_FOREACH_SAFE(sceneData->primitives, primitive, tempPrimitive) {
    vulkan_data_primitive_deinit(primitive);
    core_free(primitive);
  }

  vulkan_data_object *object = NULL;
  while ((object = (utarray_next(sceneData->objects, object)))) {
    vulkan_data_object_deinit(object);
  }
  utarray_free(sceneData->objects);

  vulkan_data_camera *camera = NULL;
  while ((camera = (utarray_next(sceneData->cameras, camera)))) {
    vulkan_data_camera_deinit(camera);
  }
  utarray_free(sceneData->cameras);

  core_free(sceneData);
}

#define GET_OBJECT_BY_KEY(_type, ...)                                                              \
  vulkan_data_##_type *vulkan_data_scene_get_##_type##_by_key(                                     \
      vulkan_data_scene *sceneData, data_asset_db *assetDb, data_key key) {                        \
    vulkan_data_##_type *object = NULL;                                                            \
    vulkan_data_##_type *existingObject = NULL;                                                    \
    DL_FOREACH(sceneData->_type##s, existingObject) {                                              \
      if (existingObject->hash.value == key.value) {                                               \
        object = existingObject;                                                                   \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
    if (object == NULL && assetDb != NULL) {                                                       \
      vulkan_data_##_type *newObject = core_alloc(sizeof(vulkan_data_##_type));                    \
      vulkan_data_##_type##_init(newObject, ##__VA_ARGS__);                                        \
      vulkan_data_##_type##_deserialize(newObject, assetDb, key);                                  \
      LL_PREPEND(sceneData->_type##s, newObject);                                                  \
      object = sceneData->_type##s;                                                                \
    }                                                                                              \
    return object;                                                                                 \
  }

GET_OBJECT_BY_KEY(image)
GET_OBJECT_BY_KEY(sampler)
GET_OBJECT_BY_KEY(texture, sceneData)
GET_OBJECT_BY_KEY(material, sceneData)
GET_OBJECT_BY_KEY(primitive, sceneData)

vulkan_data_scene *vulkan_data_scene_create_with_gltf_file(UT_string *sceneName,
                                                           UT_string *gltfPath) {
  vulkan_data_scene *sceneData = parse_cgltf_scene(sceneName, gltfPath);
  return sceneData;
}

vulkan_data_scene *vulkan_data_scene_create_with_asset_db(data_asset_db *assetDb,
                                                          UT_string *sceneName) {
  vulkan_data_scene *sceneData = NULL;
  sceneData = vulkan_data_scene_create(sceneName);
  data_key sceneKey = vulkan_data_scene_calculate_key(sceneData);
  vulkan_data_scene_deserialize(sceneData, assetDb, sceneKey);
  return sceneData;
}

/* debug print */

void vulkan_data_scene_debug_print(vulkan_data_scene *sceneData) {
  log_debug("SCENE_DATA:\n");
  vulkan_data_object *object = NULL;
  while ((object = (utarray_next(sceneData->objects, object)))) {
    vulkan_data_object_debug_print(object);
  }
}
