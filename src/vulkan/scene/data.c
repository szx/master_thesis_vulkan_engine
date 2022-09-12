#include "data.h"

/* glTF parsing */

VkPrimitiveTopology cgltf_primitive_type_topology(cgltf_primitive_type type) {
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
    UNREACHABLE;
  }
}

vertex_attribute_type cgltf_to_vertex_attribute_type(cgltf_attribute_type type) {
  switch (type) {
  case cgltf_attribute_type_position:
    return vertex_attribute_type_position;
  case cgltf_attribute_type_normal:
    return vertex_attribute_type_normal;
  case cgltf_attribute_type_tangent:
    return vertex_attribute_type_tangent;
  case cgltf_attribute_type_texcoord:
    return vertex_attribute_type_texcoord;
  case cgltf_attribute_type_color:
    return vertex_attribute_type_color;
  case cgltf_attribute_type_joints:
  case cgltf_attribute_type_weights:
  case cgltf_attribute_type_invalid:
  default:
    UNREACHABLE;
  }
}

asset_image *parse_cgltf_image(scene_data *sceneData, cgltf_image *cgltfImage, image_type type) {
  if (cgltfImage == NULL) {
    return sceneData->images;
  }
  asset_image *image = core_alloc(sizeof(asset_image));
  asset_image_init(image, sceneData);

  UT_string *imagePath = get_path_dirname(sceneData->path);
  append_to_path(imagePath, cgltfImage->uri);
  log_debug("%s", utstring_body(imagePath));
  int w, h, c;
  stbi_uc *pixels = stbi_load(utstring_body(imagePath), &w, &h, &c, STBI_default);
  utstring_free(imagePath);
  image->width = w;
  image->height = h;
  image->depth = 1;
  image->channels = c;
  image->faceCount = 1;
  image->type = type;
  size_t imageSize =
      image->width * image->height * image->depth * image->channels * image->faceCount;
  utarray_resize(image->data, imageSize);
  core_memcpy(utarray_front(image->data), pixels, imageSize);
  core_free(pixels);

  return scene_data_add_image(sceneData, image);
}

asset_sampler *parse_cgltf_sampler(scene_data *sceneData, cgltf_sampler *cgltfSampler) {
  if (cgltfSampler == NULL) {
    return sceneData->samplers;
  }
  asset_sampler *sampler = core_alloc(sizeof(asset_sampler));
  asset_sampler_init(sampler, sceneData);
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

  return scene_data_add_sampler(sceneData, sampler);
}

asset_texture *parse_cgltf_texture(scene_data *sceneData, cgltf_texture *cgltfTexture,
                                   image_type type) {
  if (cgltfTexture == NULL) {
    return sceneData->textures;
  }
  asset_texture *texture = core_alloc(sizeof(asset_texture));
  asset_texture_init(texture, sceneData);

  texture->image = parse_cgltf_image(sceneData, cgltfTexture->image, type);
  texture->sampler = parse_cgltf_sampler(sceneData, cgltfTexture->sampler);

  return scene_data_add_texture(sceneData, texture);
}

asset_material *parse_cgltf_material(scene_data *sceneData, cgltf_material *cgltfMaterial) {
  if (cgltfMaterial == NULL) {
    return sceneData->materials;
  }
  asset_material *material = core_alloc(sizeof(asset_material));
  asset_material_init(material, sceneData);
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
        sceneData, cgltfMaterial->pbr_metallic_roughness.base_color_texture.texture,
        image_type_material_base_color);
    material->metallicRoughnessTexture = parse_cgltf_texture(
        sceneData, cgltfMaterial->pbr_metallic_roughness.metallic_roughness_texture.texture,
        image_type_material_parameters);
    material->normalMapTexture = parse_cgltf_texture(
        sceneData, cgltfMaterial->normal_texture.texture, image_type_material_normal_map);
  }

  return scene_data_add_material(sceneData, material);
}

asset_vertex_attribute *parse_cgltf_vertex_attribute(scene_data *sceneData,
                                                     cgltf_accessor *cgltfAccessor) {
  if (cgltfAccessor == NULL) {
    verify(sceneData->vertexAttributes != NULL);
    return sceneData->vertexAttributes;
  }
  asset_vertex_attribute *vertexAttribute = core_alloc(sizeof(asset_vertex_attribute));
  asset_vertex_attribute_init(vertexAttribute, sceneData);

  if (cgltfAccessor->type == cgltf_type_scalar) {
    utarray_realloc(vertexAttribute->data, sizeof(uint32_t));
    utarray_resize(vertexAttribute->data, cgltfAccessor->count);
    for (size_t idx = 0; idx < utarray_len(vertexAttribute->data); idx++) {
      uint32_t indexValue = cgltf_accessor_read_index(cgltfAccessor, idx);
      *(uint32_t *)utarray_eltptr(vertexAttribute->data, idx) = indexValue;
    }
    vertexAttribute->componentType = asset_vertex_attribute_component_uint32_t;
  } else if (cgltfAccessor->type == cgltf_type_vec2) {
    utarray_realloc(vertexAttribute->data, sizeof(vec2));
    utarray_resize(vertexAttribute->data, cgltfAccessor->count);
    for (size_t idx = 0; idx < utarray_len(vertexAttribute->data); idx++) {
      verify(cgltf_accessor_read_float(cgltfAccessor, idx,
                                       (float *)utarray_eltptr(vertexAttribute->data, idx), 2));
    }
    vertexAttribute->componentType = asset_vertex_attribute_component_vec2;
  } else if (cgltfAccessor->type == cgltf_type_vec3) {
    utarray_realloc(vertexAttribute->data, sizeof(vec3));
    utarray_resize(vertexAttribute->data, cgltfAccessor->count);
    for (size_t idx = 0; idx < utarray_len(vertexAttribute->data); idx++) {
      verify(cgltf_accessor_read_float(cgltfAccessor, idx,
                                       (float *)utarray_eltptr(vertexAttribute->data, idx), 3));
    }
    vertexAttribute->componentType = asset_vertex_attribute_component_vec3;
  } else if (cgltfAccessor->type == cgltf_type_vec4) {
    utarray_realloc(vertexAttribute->data, sizeof(vec4));
    utarray_resize(vertexAttribute->data, cgltfAccessor->count);
    for (size_t idx = 0; idx < utarray_len(vertexAttribute->data); idx++) {
      verify(cgltf_accessor_read_float(cgltfAccessor, idx,
                                       (float *)utarray_eltptr(vertexAttribute->data, idx), 4));
    }
    vertexAttribute->componentType = asset_vertex_attribute_component_vec4;
  }

  return scene_data_add_vertex_attribute(sceneData, vertexAttribute);
}

asset_primitive *parse_cgltf_primitive(scene_data *sceneData, cgltf_primitive *cgltfPrimitive) {
  // Check if glTF uses only supported capabilities.
  assert(cgltfPrimitive->has_draco_mesh_compression == false);
  assert(cgltfPrimitive->targets_count == 0);
  assert(cgltfPrimitive->mappings_count == 0);

  VkPrimitiveTopology topology = cgltf_primitive_type_topology(cgltfPrimitive->type);
  cgltf_accessor *cgltfIndices = cgltfPrimitive->indices;
  uint32_t vertexCount = 0;
  for (size_t attributeIdx = 0; attributeIdx < cgltfPrimitive->attributes_count; attributeIdx++) {
    cgltf_attribute *cgltfAttribute = &cgltfPrimitive->attributes[attributeIdx];
    vertex_attribute_type type = cgltf_to_vertex_attribute_type(cgltfAttribute->type);
    if (vertexCount == 0) {
      vertexCount = cgltfAttribute->data->count;
    }
    verify(vertexCount == cgltfAttribute->data->count);
  }

  asset_primitive *primitive = core_alloc(sizeof(asset_primitive));
  asset_primitive_init(primitive, sceneData);

  asset_material *material = parse_cgltf_material(sceneData, cgltfPrimitive->material);
  primitive->material = material;

  primitive->topology = topology;
  primitive->vertexCount = vertexCount;

  primitive->indices = parse_cgltf_vertex_attribute(sceneData, cgltfIndices);

  for (size_t attributeIdx = 0; attributeIdx < cgltfPrimitive->attributes_count; attributeIdx++) {
    cgltf_attribute *cgltfAttribute = &cgltfPrimitive->attributes[attributeIdx];
    vertex_attribute_type type = cgltf_to_vertex_attribute_type(cgltfAttribute->type);
    if (type == vertex_attribute_type_position) {
      primitive->positions = parse_cgltf_vertex_attribute(sceneData, cgltfAttribute->data);
    } else if (type == vertex_attribute_type_normal) {
      primitive->normals = parse_cgltf_vertex_attribute(sceneData, cgltfAttribute->data);
    } else if (type == vertex_attribute_type_color) {
      primitive->colors = parse_cgltf_vertex_attribute(sceneData, cgltfAttribute->data);
    } else if (type == vertex_attribute_type_texcoord) {
      primitive->texCoords = parse_cgltf_vertex_attribute(sceneData, cgltfAttribute->data);
    } else if (type == vertex_attribute_type_tangent) {
      primitive->tangents = parse_cgltf_vertex_attribute(sceneData, cgltfAttribute->data);
    } else {
      vertex_attribute_type_debug_print(type, 0);
      log_warn("unsupported vertex attribute type, skipping");
    }
  }
  primitive->indices = (primitive->indices == NULL)
                           ? scene_data_get_default_vertex_attribute(sceneData)
                           : primitive->indices;
  primitive->positions = (primitive->positions == NULL)
                             ? scene_data_get_default_vertex_attribute(sceneData)
                             : primitive->positions;
  primitive->normals = (primitive->normals == NULL)
                           ? scene_data_get_default_vertex_attribute(sceneData)
                           : primitive->normals;
  primitive->colors = (primitive->colors == NULL)
                          ? scene_data_get_default_vertex_attribute(sceneData)
                          : primitive->colors;
  primitive->texCoords = (primitive->texCoords == NULL)
                             ? scene_data_get_default_vertex_attribute(sceneData)
                             : primitive->texCoords;
  primitive->tangents = (primitive->tangents == NULL)
                            ? scene_data_get_default_vertex_attribute(sceneData)
                            : primitive->tangents;
  return scene_data_add_primitive(sceneData, primitive);
}

asset_mesh *parse_cgltf_mesh(scene_data *sceneData, cgltf_mesh *cgltfMesh) {
  if (cgltfMesh == NULL) {
    return NULL;
  }

  asset_mesh *mesh = core_alloc(sizeof(asset_mesh));
  asset_mesh_init(mesh, sceneData);

  for (size_t primitiveIdx = 0; primitiveIdx < cgltfMesh->primitives_count; primitiveIdx++) {
    asset_primitive *primitive =
        parse_cgltf_primitive(sceneData, &cgltfMesh->primitives[primitiveIdx]);
    utarray_push_back(mesh->primitives, &primitive);
  }

  mesh->key = asset_mesh_calculate_key(mesh);

  return mesh;
}

asset_camera *parse_cgltf_camera(scene_data *sceneData, cgltf_camera *cgltfCamera) {
  if (cgltfCamera == NULL) {
    return NULL;
  }

  asset_camera *camera = core_alloc(sizeof(asset_camera));
  asset_camera_init(camera, sceneData);

  if (cgltfCamera->type == cgltf_camera_type_perspective) {
    camera->type = camera_type_perspective;
    camera->fovY = cgltfCamera->data.perspective.yfov;
    camera->aspectRatio = cgltfCamera->data.perspective.aspect_ratio;
    camera->nearZ = cgltfCamera->data.perspective.znear;
    camera->farZ = cgltfCamera->data.perspective.zfar;
  } else if (cgltfCamera->type == cgltf_camera_type_orthographic) {
    camera->type = camera_type_orthographic;
    camera->magX = cgltfCamera->data.orthographic.xmag;
    camera->magY = cgltfCamera->data.orthographic.ymag;
    camera->nearZ = cgltfCamera->data.orthographic.znear;
    camera->farZ = cgltfCamera->data.orthographic.zfar;
  } else {
    UNREACHABLE;
  }

  camera->key = asset_camera_calculate_key(camera);

  return camera;
}

asset_object *parse_cgltf_node(scene_data *sceneData, cgltf_node *cgltfNode) {
  assert(cgltfNode->extras.start_offset == 0 && cgltfNode->extras.end_offset == 0);
  assert(cgltfNode->light == NULL);
  assert(cgltfNode->weights_count == 0);

  cgltf_mesh *cgltfMesh = cgltfNode->mesh;
  cgltf_camera *cgltfCamera = cgltfNode->camera;

  asset_object *object = core_alloc(sizeof(asset_object));
  asset_object_init(object, sceneData);

  object->mesh = parse_cgltf_mesh(sceneData, cgltfMesh);
  object->camera = parse_cgltf_camera(sceneData, cgltfCamera);

  // TODO: Animation, will probably require cgltf_node_transform_local().
  cgltf_node_transform_local(cgltfNode, (float *)object->transform);
  for (size_t childIdx = 0; childIdx < cgltfNode->children_count; childIdx++) {
    asset_object *child = parse_cgltf_node(sceneData, cgltfNode->children[childIdx]);
    utarray_push_back(object->children, &child);
  }

  return scene_data_add_object(sceneData, object);
}

asset_skybox *parse_config_skybox(scene_data *sceneData, data_config *sceneConfig,
                                  data_asset_db *assetDb) {
  asset_skybox skybox; // FIXME: Deserialize smarter, key == skyboxName.
  asset_skybox_init(&skybox, sceneData);
  utstring_printf(skybox.name, "%s", utstring_body(sceneConfig->asset.skyboxName));
  data_key skyboxKey = asset_skybox_calculate_key(&skybox);
  asset_skybox *result = scene_data_get_skybox_by_key(sceneData, assetDb, skyboxKey);
  asset_skybox_deinit(&skybox);
  return result;
}

scene_data *parse_cgltf_scene(UT_string *name, UT_string *path, UT_string *sceneConfigPath,
                              data_config *assetConfig, data_asset_db *assetDb) {
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

  scene_data *sceneData = scene_data_create(name, assetConfig);
  utstring_concat(sceneData->path, path);

  // parse objects
  for (size_t objectIdx = 0; objectIdx < cgltfScene->nodes_count; objectIdx++) {
    asset_object *object = parse_cgltf_node(sceneData, cgltfScene->nodes[objectIdx]);
    utarray_push_back(sceneData->rootObjects, &object);
  }

  cgltf_free(cgltfData);

  data_config *sceneConfig = data_config_create(sceneConfigPath, data_config_type_asset);

  // parse skybox
  sceneData->skybox = parse_config_skybox(sceneData, sceneConfig, assetDb);

  data_config_destroy(sceneConfig);

  return sceneData;
}

/* scene data */

#define DEF_GET_VULKAN_ASSET_BY_KEY(_type, _var)                                                   \
  asset_##_type *scene_data_get_##_type##_by_key(scene_data *sceneData, data_asset_db *assetDb,    \
                                                 data_key key) {                                   \
    asset_##_type *entity = NULL;                                                                  \
    dl_foreach_elem(asset_##_type *, existingEntity, sceneData->_var) {                            \
      if (existingEntity->key.value == key.value) {                                                \
        entity = existingEntity;                                                                   \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
    if (entity == NULL && assetDb != NULL) {                                                       \
      asset_##_type *newObject = core_alloc(sizeof(asset_##_type));                                \
      asset_##_type##_init(newObject, sceneData);                                                  \
      asset_##_type##_deserialize(newObject, assetDb, key);                                        \
      DL_APPEND(sceneData->_var, newObject);                                                       \
      entity = newObject;                                                                          \
    }                                                                                              \
    return entity;                                                                                 \
  }

#define DEF_GET_VULKAN_DEFAULT_ASSET(_type, _var)                                                  \
  asset_##_type *scene_data_get_default_##_type(scene_data *sceneData) { return sceneData->_var; }

#define DEF_ADD_VULKAN_ENTITY(_type, _var)                                                         \
  asset_##_type *scene_data_add_##_type(scene_data *sceneData, asset_##_type *entity) {            \
                                                                                                   \
    entity->key = asset_##_type##_calculate_key(entity);                                           \
    asset_##_type *existingEntity = scene_data_get_##_type##_by_key(sceneData, NULL, entity->key); \
    if (existingEntity != NULL) {                                                                  \
      asset_##_type##_deinit(entity);                                                              \
      core_free(entity);                                                                           \
      return existingEntity;                                                                       \
    }                                                                                              \
    log_debug("adding new " #_type);                                                               \
    DL_APPEND(sceneData->_var, entity);                                                            \
    return entity;                                                                                 \
  }

#define VULKAN_ASSET_FIELD_DEFS_FUNCS(_type, _var)                                                 \
  DEF_GET_VULKAN_ASSET_BY_KEY(_type, _var)                                                         \
  DEF_GET_VULKAN_DEFAULT_ASSET(_type, _var) DEF_ADD_VULKAN_ENTITY(_type, _var)

VULKAN_ASSET_FIELD_DEFS_FUNCS(image, images)
VULKAN_ASSET_FIELD_DEFS_FUNCS(sampler, samplers)
VULKAN_ASSET_FIELD_DEFS_FUNCS(texture, textures)
VULKAN_ASSET_FIELD_DEFS_FUNCS(direct_light, directLights)
VULKAN_ASSET_FIELD_DEFS_FUNCS(skybox, skyboxes)
VULKAN_ASSET_FIELD_DEFS_FUNCS(font, fonts)
VULKAN_ASSET_FIELD_DEFS_FUNCS(material, materials)
VULKAN_ASSET_FIELD_DEFS_FUNCS(vertex_attribute, vertexAttributes)
VULKAN_ASSET_FIELD_DEFS_FUNCS(primitive, primitives)
VULKAN_ASSET_FIELD_DEFS_FUNCS(object, objects)

#undef DEF_GET_VULKAN_ASSET_BY_KEY
#undef DEF_ADD_VULKAN_ENTITY
#undef VULKAN_ASSET_FIELD_DEFS_FUNCS
data_key scene_data_calculate_key(scene_data *sceneData) {
  hash_t value;
  HASH_START(hashState)
  HASH_UPDATE(hashState, utstring_body(sceneData->name), utstring_len(sceneData->name))
  HASH_DIGEST(hashState, value)
  HASH_END(hashState);
  return (data_key){value};
}

void scene_data_serialize(scene_data *sceneData, data_asset_db *assetDb) {
  // scene_data_debug_print(sceneData);
  UT_array *objectKeys = NULL;
  utarray_alloc(objectKeys, sizeof(data_key));
  utarray_foreach_elem_deref (asset_object *, rootObject, sceneData->rootObjects) {
    asset_object_serialize(rootObject, assetDb);
    utarray_push_back(objectKeys, &rootObject->key);
  }
  data_asset_db_insert_scene_name_text(assetDb, sceneData->key, (data_text){sceneData->name});
  data_asset_db_insert_scene_objects_key_array(assetDb, sceneData->key,
                                               data_key_array_temp(objectKeys));
  utarray_free(objectKeys);

  data_asset_db_insert_scene_skybox_key(assetDb, sceneData->key, sceneData->skybox->key);

  UT_array *directLightKeys = NULL;
  utarray_alloc(directLightKeys, sizeof(data_key));
  dl_foreach_elem(asset_direct_light *, directLight, sceneData->directLights) {
    if (directLight == scene_data_get_default_direct_light(sceneData)) {
      continue;
    }
    asset_direct_light_serialize(directLight, assetDb);
    utarray_push_back(directLightKeys, &directLight->key);
  }
  data_asset_db_insert_scene_directLights_key_array(assetDb, sceneData->key,
                                                    data_key_array_temp(directLightKeys));
  utarray_free(directLightKeys);
}

void scene_data_deserialize(scene_data *sceneData, data_asset_db *assetDb, data_key key) {
  sceneData->key = key;

  data_key_array objectKeyArray =
      data_asset_db_select_scene_objects_key_array(assetDb, sceneData->key);
  utarray_foreach_elem_deref (data_key, objectKey, objectKeyArray.values) {
    asset_object *object = scene_data_get_object_by_key(sceneData, assetDb, objectKey);
    utarray_push_back(sceneData->rootObjects, &object);
  }
  data_key_array_deinit(&objectKeyArray);

  sceneData->skybox = scene_data_get_skybox_by_key(
      sceneData, assetDb, data_asset_db_select_scene_skybox_key(assetDb, sceneData->key));

  // FIXME: Deserialize smarter, key == fontName.
  asset_font font;
  asset_font_init(&font, sceneData);
  utstring_printf(font.name, "%s", utstring_body(sceneData->assetConfig->global.graphicsFont));
  data_key fontKey = asset_font_calculate_key(&font);
  sceneData->font = scene_data_get_font_by_key(sceneData, assetDb, fontKey);
  asset_font_deinit(&font);

  data_key_array directLightKeyArray =
      data_asset_db_select_scene_directLights_key_array(assetDb, sceneData->key);
  utarray_foreach_elem_deref (data_key, directLightKey, directLightKeyArray.values) {
    asset_direct_light *directLight =
        scene_data_get_direct_light_by_key(sceneData, assetDb, directLightKey);
  }
  data_key_array_deinit(&directLightKeyArray);
}

scene_data *scene_data_create(UT_string *name, data_config *assetConfig) {
  scene_data *sceneData = core_alloc(sizeof(scene_data));

  sceneData->assetConfig = assetConfig;

  utstring_new(sceneData->name);
  utstring_concat(sceneData->name, name);

  utstring_new(sceneData->path);

  sceneData->images = NULL; // NOTE: We use default image.
  asset_image *defaultImage = core_alloc(sizeof(asset_image));
  asset_image_init(defaultImage, sceneData);
  DL_APPEND(sceneData->images, defaultImage);

  sceneData->samplers = NULL; // NOTE: We use default sampler.
  asset_sampler *defaultSampler = core_alloc(sizeof(asset_sampler));
  asset_sampler_init(defaultSampler, sceneData);
  DL_APPEND(sceneData->samplers, defaultSampler);

  sceneData->textures = NULL; // NOTE: We use default texture.
  asset_texture *defaultTexture = core_alloc(sizeof(asset_texture));
  asset_texture_init(defaultTexture, sceneData);
  defaultTexture->image = defaultImage;
  defaultTexture->sampler = defaultSampler;
  defaultTexture->key = asset_texture_calculate_key(defaultTexture);
  DL_APPEND(sceneData->textures, defaultTexture);

  sceneData->directLights = NULL; // NOTE: We use default direct light.
  asset_direct_light *defaultDirectLight =
      asset_direct_light_create_point_light(sceneData, (vec3){0, 0, 0}, 0, 0, (vec3){0, 0, 0});
  defaultDirectLight->key = asset_direct_light_calculate_key(defaultDirectLight);
  DL_APPEND(sceneData->directLights, defaultDirectLight);

  sceneData->skyboxes = NULL; // NOTE: We use default skybox.
  asset_skybox *defaultSkybox = core_alloc(sizeof(asset_skybox));
  asset_skybox_init(defaultSkybox, sceneData);
  defaultSkybox->cubemapTexture = defaultTexture;
  defaultSkybox->key = asset_skybox_calculate_key(defaultSkybox);
  DL_APPEND(sceneData->skyboxes, defaultSkybox);

  sceneData->fonts = NULL; // NOTE: We use default font.
  asset_font *defaultFont = core_alloc(sizeof(asset_font));
  asset_font_init(defaultFont, sceneData);
  defaultFont->fontTexture = defaultTexture;
  defaultFont->key = asset_font_calculate_key(defaultFont);
  DL_APPEND(sceneData->fonts, defaultFont);

  sceneData->materials = NULL; // NOTE: We use default material.
  asset_material *defaultMaterial = core_alloc(sizeof(asset_material));
  asset_material_init(defaultMaterial, sceneData);
  defaultMaterial->baseColorTexture = defaultTexture;
  defaultMaterial->metallicRoughnessTexture = defaultTexture;
  defaultMaterial->normalMapTexture = defaultTexture;
  defaultMaterial->key = asset_material_calculate_key(defaultMaterial);
  DL_APPEND(sceneData->materials, defaultMaterial);

  sceneData->vertexAttributes = NULL; // NOTE: We use default vertex attribute.
  asset_vertex_attribute *defaultVertexAttribute = core_alloc(sizeof(asset_vertex_attribute));
  asset_vertex_attribute_init(defaultVertexAttribute, sceneData);
  DL_APPEND(sceneData->vertexAttributes, defaultVertexAttribute);

  sceneData->primitives = NULL;

  sceneData->objects = NULL;

  utarray_alloc(sceneData->rootObjects, sizeof(asset_object *));
  sceneData->skybox = defaultSkybox;

  sceneData->font = defaultFont;

  asset_camera_init(&sceneData->defaultCamera, sceneData);

  sceneData->key = scene_data_calculate_key(sceneData);
  return sceneData;
}

void scene_data_destroy(scene_data *sceneData) {
  utstring_free(sceneData->name);
  utstring_free(sceneData->path);

  dl_foreach_elem(asset_image *, image, sceneData->images) {
    asset_image_deinit(image);
    core_free(image);
  }

  dl_foreach_elem(asset_sampler *, sampler, sceneData->samplers) {
    asset_sampler_deinit(sampler);
    core_free(sampler);
  }

  dl_foreach_elem(asset_texture *, texture, sceneData->textures) {
    asset_texture_deinit(texture);
    core_free(texture);
  }

  dl_foreach_elem(asset_direct_light *, directLight, sceneData->directLights) {
    asset_direct_light_deinit(directLight);
    core_free(directLight);
  }

  dl_foreach_elem(asset_skybox *, skybox, sceneData->skyboxes) {
    asset_skybox_deinit(skybox);
    core_free(skybox);
  }

  dl_foreach_elem(asset_font *, font, sceneData->fonts) {
    asset_font_deinit(font);
    core_free(font);
  }

  dl_foreach_elem(asset_material *, material, sceneData->materials) {
    asset_material_deinit(material);
    core_free(material);
  }

  dl_foreach_elem(asset_vertex_attribute *, vertexAttribute, sceneData->vertexAttributes) {
    asset_vertex_attribute_deinit(vertexAttribute);
    core_free(vertexAttribute);
  }

  dl_foreach_elem(asset_primitive *, primitive, sceneData->primitives) {
    asset_primitive_deinit(primitive);
    core_free(primitive);
  }

  dl_foreach_elem(asset_object *, object, sceneData->objects) {
    asset_object_deinit(object);
    core_free(object);
  }

  utarray_free(sceneData->rootObjects);

  asset_camera_deinit(&sceneData->defaultCamera);

  core_free(sceneData);
}

scene_data *scene_data_create_with_gltf_file(UT_string *sceneName, UT_string *gltfPath,
                                             UT_string *sceneConfigPath, data_config *assetConfig,
                                             data_asset_db *assetDb) {
  scene_data *sceneData =
      parse_cgltf_scene(sceneName, gltfPath, sceneConfigPath, assetConfig, assetDb);
  return sceneData;
}

scene_data *scene_data_create_with_asset_db(data_config *assetConfig, data_asset_db *assetDb,
                                            UT_string *sceneName) {
  scene_data *sceneData = NULL;
  sceneData = scene_data_create(sceneName, assetConfig);
  data_key sceneKey = scene_data_calculate_key(sceneData);
  scene_data_deserialize(sceneData, assetDb, sceneKey);
  return sceneData;
}

void scene_data_debug_print(scene_data *sceneData, int indent) {
  log_debug(INDENT_FORMAT_STRING "SCENE_DATA:", INDENT_FORMAT_ARGS(0));
  size_t i = 0;
  utarray_foreach_elem_deref (asset_object *, object, sceneData->rootObjects) {
    log_debug(INDENT_FORMAT_STRING "root object #%d", INDENT_FORMAT_ARGS(2), i++);
    asset_object_debug_print(object, indent + 2);
  }
}
