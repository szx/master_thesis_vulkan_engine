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
  vulkan_data_image *image = core_alloc(sizeof(vulkan_data_image));
  vulkan_data_image_init(image, sceneData);

  UT_string *imagePath = get_path_dirname(sceneData->path);
  append_to_path(imagePath, cgltfImage->uri);
  log_debug("%s", utstring_body(imagePath));
  int w, h, c;
  stbi_uc *pixels = stbi_load(utstring_body(imagePath), &w, &h, &c, STBI_rgb_alpha);
  utstring_free(imagePath);
  image->width = w;
  image->height = h;
  image->depth = 1;
  image->channels = c;
  size_t imageSize = image->width * image->height * image->depth * image->channels;
  utarray_resize(image->data, imageSize);
  core_memcpy(utarray_front(image->data), pixels, imageSize);
  core_free(pixels);

  return vulkan_data_scene_add_image(sceneData, image);
}

vulkan_data_sampler *parse_cgltf_sampler(vulkan_data_scene *sceneData,
                                         cgltf_sampler *cgltfSampler) {
  if (cgltfSampler == NULL) {
    return sceneData->samplers;
  }
  vulkan_data_sampler *sampler = core_alloc(sizeof(vulkan_data_sampler));
  vulkan_data_sampler_init(sampler, sceneData);
  sampler->magFilter = cgltfSampler->mag_filter == 9728 ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
  sampler->minFilter = cgltfSampler->min_filter == 9728 ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
  sampler->addressModeU =
      cgltfSampler->wrap_s == 33071
          ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
          : (cgltfSampler->wrap_s == 33648 ? VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT
                                           : VK_SAMPLER_ADDRESS_MODE_REPEAT);
  sampler->addressModeV =
      cgltfSampler->wrap_t == 33071
          ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
          : (cgltfSampler->wrap_s == 33648 ? VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT
                                           : VK_SAMPLER_ADDRESS_MODE_REPEAT);

  return vulkan_data_scene_add_sampler(sceneData, sampler);
}

vulkan_data_texture *parse_cgltf_texture(vulkan_data_scene *sceneData,
                                         cgltf_texture *cgltfTexture) {
  if (cgltfTexture == NULL) {
    return sceneData->textures;
  }
  vulkan_data_texture *texture = core_alloc(sizeof(vulkan_data_texture));
  vulkan_data_texture_init(texture, sceneData);

  texture->image = parse_cgltf_image(sceneData, cgltfTexture->image);
  texture->sampler = parse_cgltf_sampler(sceneData, cgltfTexture->sampler);

  return vulkan_data_scene_add_texture(sceneData, texture);
}

vulkan_data_material *parse_cgltf_material(vulkan_data_scene *sceneData,
                                           cgltf_material *cgltfMaterial) {
  if (cgltfMaterial == NULL) {
    return sceneData->materials;
  }
  vulkan_data_material *material = core_alloc(sizeof(vulkan_data_material));
  vulkan_data_material_init(material, sceneData);
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
    material->baseColorFactor[0] = cgltfMaterial->pbr_metallic_roughness.base_color_factor[0];
    material->baseColorFactor[1] = cgltfMaterial->pbr_metallic_roughness.base_color_factor[1];
    material->baseColorFactor[2] = cgltfMaterial->pbr_metallic_roughness.base_color_factor[2];
    material->baseColorFactor[3] = cgltfMaterial->pbr_metallic_roughness.base_color_factor[3];
    material->metallicFactor = cgltfMaterial->pbr_metallic_roughness.metallic_factor;
    material->roughnessFactor = cgltfMaterial->pbr_metallic_roughness.roughness_factor;

    material->baseColorTexture = parse_cgltf_texture(
        sceneData, cgltfMaterial->pbr_metallic_roughness.base_color_texture.texture);
    material->metallicRoughnessTexture = parse_cgltf_texture(
        sceneData, cgltfMaterial->pbr_metallic_roughness.metallic_roughness_texture.texture);
  }

  return vulkan_data_scene_add_material(sceneData, material);
}

vulkan_data_vertex_attribute *parse_cgltf_vertex_attribute(vulkan_data_scene *sceneData,
                                                           cgltf_accessor *cgltfAccessor) {
  if (cgltfAccessor == NULL) {
    verify(sceneData->vertexAttributes != NULL);
    return sceneData->vertexAttributes;
  }
  vulkan_data_vertex_attribute *vertexAttribute = core_alloc(sizeof(vulkan_data_vertex_attribute));
  vulkan_data_vertex_attribute_init(vertexAttribute, sceneData);

  if (cgltfAccessor->type == cgltf_type_scalar) {
    utarray_realloc(vertexAttribute->data, sizeof(uint32_t));
    utarray_resize(vertexAttribute->data, cgltfAccessor->count);
    for (size_t idx = 0; idx < utarray_len(vertexAttribute->data); idx++) {
      uint32_t indexValue = cgltf_accessor_read_index(cgltfAccessor, idx);
      *(uint32_t *)utarray_eltptr(vertexAttribute->data, idx) = indexValue;
    }
    vertexAttribute->componentType = vulkan_data_vertex_attribute_component_uint32_t;
  } else if (cgltfAccessor->type == cgltf_type_vec2) {
    utarray_realloc(vertexAttribute->data, sizeof(vec2));
    utarray_resize(vertexAttribute->data, cgltfAccessor->count);
    for (size_t idx = 0; idx < utarray_len(vertexAttribute->data); idx++) {
      verify(cgltf_accessor_read_float(cgltfAccessor, idx,
                                       (float *)utarray_eltptr(vertexAttribute->data, idx), 2));
    }
    vertexAttribute->componentType = vulkan_data_vertex_attribute_component_vec2;
  } else if (cgltfAccessor->type == cgltf_type_vec3) {
    utarray_realloc(vertexAttribute->data, sizeof(vec3));
    utarray_resize(vertexAttribute->data, cgltfAccessor->count);
    for (size_t idx = 0; idx < utarray_len(vertexAttribute->data); idx++) {
      verify(cgltf_accessor_read_float(cgltfAccessor, idx,
                                       (float *)utarray_eltptr(vertexAttribute->data, idx), 3));
    }
    vertexAttribute->componentType = vulkan_data_vertex_attribute_component_vec3;
  }

  return vulkan_data_scene_add_vertex_attribute(sceneData, vertexAttribute);
}

vulkan_data_primitive *parse_cgltf_primitive(vulkan_data_scene *sceneData,
                                             cgltf_primitive *cgltfPrimitive) {
  // Check if glTF uses only supported capabilities.
  assert(cgltfPrimitive->has_draco_mesh_compression == false);
  assert(cgltfPrimitive->targets_count == 0);
  assert(cgltfPrimitive->mappings_count == 0);

  VkPrimitiveTopology topology = cgltf_primitive_type_vulkan_topology(cgltfPrimitive->type);
  cgltf_accessor *cgltfIndices = cgltfPrimitive->indices;
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

  primitive->indices = parse_cgltf_vertex_attribute(sceneData, cgltfIndices);

  for (size_t attributeIdx = 0; attributeIdx < cgltfPrimitive->attributes_count; attributeIdx++) {
    cgltf_attribute *cgltfAttribute = &cgltfPrimitive->attributes[attributeIdx];
    vulkan_attribute_type type = cgltf_to_vulkan_attribute_type(cgltfAttribute->type);
    if (type == PositionAttribute) {
      primitive->positions = parse_cgltf_vertex_attribute(sceneData, cgltfAttribute->data);
    } else if (type == NormalAttribute) {
      primitive->normals = parse_cgltf_vertex_attribute(sceneData, cgltfAttribute->data);
    } else if (type == ColorAttribute) {
      primitive->colors = parse_cgltf_vertex_attribute(sceneData, cgltfAttribute->data);
    } else if (type == TexCoordAttribute) {
      primitive->texCoords = parse_cgltf_vertex_attribute(sceneData, cgltfAttribute->data);
    } else {
      vulkan_attribute_type_debug_print(type);
      log_warn("unsupported vertex attribute type, skipping");
    }
  }
  primitive->indices =
      (primitive->indices == NULL) ? sceneData->vertexAttributes : primitive->indices;
  primitive->positions =
      (primitive->positions == NULL) ? sceneData->vertexAttributes : primitive->positions;
  primitive->normals =
      (primitive->normals == NULL) ? sceneData->vertexAttributes : primitive->normals;
  primitive->colors = (primitive->colors == NULL) ? sceneData->vertexAttributes : primitive->colors;
  primitive->texCoords =
      (primitive->texCoords == NULL) ? sceneData->vertexAttributes : primitive->texCoords;

  return vulkan_data_scene_add_primitive(sceneData, primitive);
}

vulkan_data_mesh *parse_cgltf_mesh(vulkan_data_scene *sceneData, cgltf_mesh *cgltfMesh) {
  if (cgltfMesh == NULL) {
    return NULL;
  }

  vulkan_data_mesh *mesh = core_alloc(sizeof(vulkan_data_mesh));
  vulkan_data_mesh_init(mesh, sceneData);

  for (size_t primitiveIdx = 0; primitiveIdx < cgltfMesh->primitives_count; primitiveIdx++) {
    vulkan_data_primitive *primitive =
        parse_cgltf_primitive(sceneData, &cgltfMesh->primitives[primitiveIdx]);
    utarray_push_back(mesh->primitives, &primitive);
  }

  mesh->key = vulkan_data_mesh_calculate_key(mesh);

  return mesh;
}

vulkan_data_object *parse_cgltf_node(vulkan_data_scene *sceneData, cgltf_node *cgltfNode) {
  assert(cgltfNode->extras.start_offset == 0 && cgltfNode->extras.end_offset == 0);
  assert(cgltfNode->camera == NULL);
  assert(cgltfNode->light == NULL);
  assert(cgltfNode->weights_count == 0);
  cgltf_mesh *cgltfMesh = cgltfNode->mesh;

  vulkan_data_object *object = core_alloc(sizeof(vulkan_data_object));
  vulkan_data_object_init(object, sceneData);

  object->mesh = parse_cgltf_mesh(sceneData, cgltfMesh);
  // TODO: Animation, will probably require cgltf_node_transform_local().
  cgltf_node_transform_world(cgltfNode, (float *)object->transform);
  for (size_t childIdx = 0; childIdx < cgltfNode->children_count; childIdx++) {
    vulkan_data_object *child = parse_cgltf_node(sceneData, cgltfNode->children[childIdx]);
    utarray_push_back(object->children, &child);
  }

  return vulkan_data_scene_add_object(sceneData, object);
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
  for (size_t objectIdx = 0; objectIdx < cgltfScene->nodes_count; objectIdx++) {
    vulkan_data_object *object = parse_cgltf_node(sceneData, cgltfScene->nodes[objectIdx]);
    utarray_push_back(sceneData->rootObjects, &object);
  }

  // parse cameras
  vulkan_data_camera defaultCamera;
  vulkan_data_camera_init(&defaultCamera, sceneData);
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
  // vulkan_data_scene_debug_print(sceneData);
  UT_array *cameraKeys = NULL;
  utarray_alloc(cameraKeys, sizeof(data_key));
  utarray_foreach_elem_it (vulkan_data_camera *, camera, sceneData->cameras) {
    vulkan_data_camera_serialize(camera, assetDb);
    utarray_push_back(cameraKeys, &camera->key);
  }
  data_asset_db_insert_scene_cameras_key_array(assetDb, sceneData->key,
                                               data_key_array_temp(cameraKeys));
  utarray_free(cameraKeys);

  UT_array *objectKeys = NULL;
  utarray_alloc(objectKeys, sizeof(data_key));
  utarray_foreach_elem_deref (vulkan_data_object *, rootObject, sceneData->rootObjects) {
    vulkan_data_object_serialize(rootObject, assetDb);
    utarray_push_back(objectKeys, &rootObject->key);
  }
  data_asset_db_insert_scene_name_text(assetDb, sceneData->key, (data_text){sceneData->name});
  data_asset_db_insert_scene_objects_key_array(assetDb, sceneData->key,
                                               data_key_array_temp(objectKeys));
  utarray_free(objectKeys);
}

void vulkan_data_scene_deserialize(vulkan_data_scene *sceneData, data_asset_db *assetDb,
                                   data_key key) {
  sceneData->key = key;

  data_key_array cameraKeyArray =
      data_asset_db_select_scene_cameras_key_array(assetDb, sceneData->key);
  data_key_array objectKeyArray =
      data_asset_db_select_scene_objects_key_array(assetDb, sceneData->key);

  utarray_foreach_elem_deref (data_key, cameraKey, cameraKeyArray.values) {
    vulkan_data_camera camera;
    vulkan_data_camera_init(&camera, sceneData);
    vulkan_data_camera_deserialize(&camera, assetDb, cameraKey);
    utarray_push_back(sceneData->cameras, &camera);
  }
  data_key_array_deinit(&cameraKeyArray);

  utarray_foreach_elem_deref (data_key, objectKey, objectKeyArray.values) {
    /* object data */
    vulkan_data_object *object = vulkan_data_scene_get_object_by_key(sceneData, assetDb, objectKey);
    utarray_push_back(sceneData->rootObjects, &object);
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
  vulkan_data_image_init(defaultImage, sceneData);
  DL_APPEND(sceneData->images, defaultImage);

  sceneData->samplers = NULL;
  vulkan_data_sampler *defaultSampler = core_alloc(sizeof(vulkan_data_sampler));
  vulkan_data_sampler_init(defaultSampler, sceneData);
  DL_APPEND(sceneData->samplers, defaultSampler);

  sceneData->textures = NULL;
  vulkan_data_texture *defaultTexture = core_alloc(sizeof(vulkan_data_texture));
  vulkan_data_texture_init(defaultTexture, sceneData);
  defaultTexture->image = defaultImage;
  defaultTexture->sampler = defaultSampler;
  defaultTexture->key = vulkan_data_texture_calculate_key(defaultTexture);
  DL_APPEND(sceneData->textures, defaultTexture);

  sceneData->materials = NULL;
  vulkan_data_material *defaultMaterial = core_alloc(sizeof(vulkan_data_material));
  vulkan_data_material_init(defaultMaterial, sceneData);
  defaultMaterial->baseColorTexture = defaultTexture;
  defaultMaterial->metallicRoughnessTexture = defaultTexture;
  defaultMaterial->key = vulkan_data_material_calculate_key(defaultMaterial);
  DL_APPEND(sceneData->materials, defaultMaterial);

  sceneData->vertexAttributes = NULL;
  vulkan_data_vertex_attribute *defaultVertexAttribute =
      core_alloc(sizeof(vulkan_data_vertex_attribute));
  vulkan_data_vertex_attribute_init(defaultVertexAttribute, sceneData);
  DL_APPEND(sceneData->vertexAttributes, defaultVertexAttribute);

  sceneData->primitives = NULL;

  sceneData->objects = NULL;

  utarray_alloc(sceneData->cameras, sizeof(vulkan_data_camera));

  utarray_alloc(sceneData->rootObjects, sizeof(vulkan_data_object *));

  sceneData->key = vulkan_data_scene_calculate_key(sceneData);
  return sceneData;
}

void vulkan_data_scene_destroy(vulkan_data_scene *sceneData) {
  utstring_free(sceneData->name);
  utstring_free(sceneData->path);

  dl_foreach_elem (vulkan_data_image *, image, sceneData->images) {
    vulkan_data_image_deinit(image);
    core_free(image);
  }

  dl_foreach_elem (vulkan_data_sampler *, sampler, sceneData->samplers) {
    vulkan_data_sampler_deinit(sampler);
    core_free(sampler);
  }

  dl_foreach_elem (vulkan_data_texture *, texture, sceneData->textures) {
    vulkan_data_texture_deinit(texture);
    core_free(texture);
  }

  dl_foreach_elem (vulkan_data_material *, material, sceneData->materials) {
    vulkan_data_material_deinit(material);
    core_free(material);
  }

  dl_foreach_elem (vulkan_data_vertex_attribute *, vertexAttribute, sceneData->vertexAttributes) {
    vulkan_data_vertex_attribute_deinit(vertexAttribute);
    core_free(vertexAttribute);
  }

  dl_foreach_elem (vulkan_data_primitive *, primitive, sceneData->primitives) {
    vulkan_data_primitive_deinit(primitive);
    core_free(primitive);
  }

  dl_foreach_elem (vulkan_data_object *, object, sceneData->objects) {
    vulkan_data_object_deinit(object);
    core_free(object);
  }

  utarray_foreach_elem_it (vulkan_data_camera *, camera, sceneData->cameras) {
    vulkan_data_camera_deinit(camera);
  }
  utarray_free(sceneData->cameras);

  utarray_free(sceneData->rootObjects);

  core_free(sceneData);
}

#define DEF_GET_VULKAN_ENTITY_BY_KEY(_type, _var)                                                  \
  vulkan_data_##_type *vulkan_data_scene_get_##_type##_by_key(                                     \
      vulkan_data_scene *sceneData, data_asset_db *assetDb, data_key key) {                        \
    vulkan_data_##_type *entity = NULL;                                                            \
    dl_foreach_elem (vulkan_data_##_type *, existingEntity, sceneData->_var) {                     \
      if (existingEntity->key.value == key.value) {                                                \
        entity = existingEntity;                                                                   \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
    if (entity == NULL && assetDb != NULL) {                                                       \
      vulkan_data_##_type *newObject = core_alloc(sizeof(vulkan_data_##_type));                    \
      vulkan_data_##_type##_init(newObject, sceneData);                                            \
      vulkan_data_##_type##_deserialize(newObject, assetDb, key);                                  \
      DL_PREPEND(sceneData->_var, newObject);                                                      \
      entity = sceneData->_var;                                                                    \
    }                                                                                              \
    return entity;                                                                                 \
  }

#define DEF_ADD_VULKAN_ENTITY(_type, _var)                                                         \
  vulkan_data_##_type *vulkan_data_scene_add_##_type(vulkan_data_scene *sceneData,                 \
                                                     vulkan_data_##_type *entity) {                \
                                                                                                   \
    entity->key = vulkan_data_##_type##_calculate_key(entity);                                     \
    vulkan_data_##_type *existingEntity =                                                          \
        vulkan_data_scene_get_##_type##_by_key(sceneData, NULL, entity->key);                      \
    if (existingEntity != NULL) {                                                                  \
      vulkan_data_##_type##_deinit(entity);                                                        \
      core_free(entity);                                                                           \
      return existingEntity;                                                                       \
    }                                                                                              \
    log_debug("adding new " #_type);                                                               \
    DL_APPEND(sceneData->_var, entity);                                                            \
    return entity;                                                                                 \
  }

#define DEF_VULKAN_ENTITY_FUNCS(_type, _var)                                                       \
  DEF_GET_VULKAN_ENTITY_BY_KEY(_type, _var) DEF_ADD_VULKAN_ENTITY(_type, _var)

DEF_VULKAN_ENTITY_FUNCS(image, images)
DEF_VULKAN_ENTITY_FUNCS(sampler, samplers)
DEF_VULKAN_ENTITY_FUNCS(texture, textures)
DEF_VULKAN_ENTITY_FUNCS(material, materials)
DEF_VULKAN_ENTITY_FUNCS(vertex_attribute, vertexAttributes)
DEF_VULKAN_ENTITY_FUNCS(primitive, primitives)
DEF_VULKAN_ENTITY_FUNCS(object, objects)

#undef DEF_GET_VULKAN_ENTITY_BY_KEY
#undef DEF_ADD_VULKAN_ENTITY
#undef DEF_VULKAN_ENTITY_FUNCS

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
  log_debug("SCENE_DATA:");
  size_t i = 0;
  utarray_foreach_elem_deref (vulkan_data_object *, object, sceneData->rootObjects) {
    log_debug("root object #%d", i++);
    vulkan_data_object_debug_print(object, 2);
  }
}
