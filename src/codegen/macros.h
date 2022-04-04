/* This file was autogenerated. */

#pragma once

#define END_OF_DATA_CONFIG_SECTION
#define DATA_CONFIG_SECTIONS(X, ...) \
  X(graphics, __VA_ARGS__) \
  X(settings, __VA_ARGS__) \
  X(controls, __VA_ARGS__) \
  END_OF_DATA_CONFIG_SECTION

#define END_OF_DATA_CONFIG_INT_KEYS
#define DATA_CONFIG_INT_KEYS(X, ...) \
  X(graphics, WindowWidth, 640, __VA_ARGS__) \
  X(graphics, WindowHeight, 480, __VA_ARGS__) \
  X(graphics, MaxInstanceCount, 1000, __VA_ARGS__) \
  X(controls, Enabled, 1, __VA_ARGS__) \
  END_OF_DATA_CONFIG_INT_KEYS

#define END_OF_DATA_CONFIG_STR_KEYS
#define DATA_CONFIG_STR_KEYS(X, ...) \
  X(graphics, WindowTitle, "cpptest", __VA_ARGS__) \
  X(settings, StartScene, "triangles", __VA_ARGS__) \
  END_OF_DATA_CONFIG_STR_KEYS

#define END_OF_DATA_DB_TYPES
#define DATA_DB_TYPES(X, ...) \
  X(byte, uint8_t, __VA_ARGS__) \
  X(int, uint32_t, __VA_ARGS__) \
  X(float, float, __VA_ARGS__) \
  X(vec2, vec2, __VA_ARGS__) \
  X(vec3, vec3, __VA_ARGS__) \
  X(vec4, vec4, __VA_ARGS__) \
  X(mat4, mat4, __VA_ARGS__) \
  X(text, UT_string *, __VA_ARGS__) \
  X(key, hash_t, __VA_ARGS__) \
  END_OF_DATA_DB_TYPES

#define END_OF_DATA_ASSET_DB_TABLES
#define DATA_ASSET_DB_TABLES(X, ...) \
  X(image, key, key, width, int, height, int, depth, int, channels, int, data, byte_array, __VA_ARGS__) \
  X(sampler, key, key, magfilter, int, minfilter, int, addresswrapu, int, addresswrapv, int, __VA_ARGS__) \
  X(texture, key, key, image, key, sampler, key, __VA_ARGS__) \
  X(material, key, key, basecolorfactor, vec4, metallicfactor, float, roughnessfactor, float, basecolortexture, key, metallicroughnesstexture, key, __VA_ARGS__) \
  X(vertexAttribute, key, key, valuesint, int_array, valuesvec2, vec2_array, valuesvec3, vec3_array, __VA_ARGS__) \
  X(primitive, key, key, material, key, topology, int, indices, key, positions, key, normals, key, colors, key, texcoords, key, __VA_ARGS__) \
  X(mesh, key, key, primitives, key_array, __VA_ARGS__) \
  X(object, key, key, transform, mat4, mesh, key, camera, key, children, key_array, __VA_ARGS__) \
  X(camera, key, key, type, int, fovy, float, aspectratio, float, nearz, float, farz, float, __VA_ARGS__) \
  X(scene, key, key, name, text, objects, key_array, cameras, key_array, __VA_ARGS__) \
  END_OF_DATA_ASSET_DB_TABLES

#define END_OF_DATA_ASSET_DB_COLUMNS
#define DATA_ASSET_DB_COLUMNS(X, ...) \
  X(image, key, key, __VA_ARGS__) \
  X(image, width, int, __VA_ARGS__) \
  X(image, height, int, __VA_ARGS__) \
  X(image, depth, int, __VA_ARGS__) \
  X(image, channels, int, __VA_ARGS__) \
  X(image, data, byte_array, __VA_ARGS__) \
  X(sampler, key, key, __VA_ARGS__) \
  X(sampler, magFilter, int, __VA_ARGS__) \
  X(sampler, minFilter, int, __VA_ARGS__) \
  X(sampler, addressWrapU, int, __VA_ARGS__) \
  X(sampler, addressWrapV, int, __VA_ARGS__) \
  X(texture, key, key, __VA_ARGS__) \
  X(texture, image, key, __VA_ARGS__) \
  X(texture, sampler, key, __VA_ARGS__) \
  X(material, key, key, __VA_ARGS__) \
  X(material, baseColorFactor, vec4, __VA_ARGS__) \
  X(material, metallicFactor, float, __VA_ARGS__) \
  X(material, roughnessFactor, float, __VA_ARGS__) \
  X(material, baseColorTexture, key, __VA_ARGS__) \
  X(material, metallicRoughnessTexture, key, __VA_ARGS__) \
  X(vertexAttribute, key, key, __VA_ARGS__) \
  X(vertexAttribute, valuesInt, int_array, __VA_ARGS__) \
  X(vertexAttribute, valuesVec2, vec2_array, __VA_ARGS__) \
  X(vertexAttribute, valuesVec3, vec3_array, __VA_ARGS__) \
  X(primitive, key, key, __VA_ARGS__) \
  X(primitive, material, key, __VA_ARGS__) \
  X(primitive, topology, int, __VA_ARGS__) \
  X(primitive, indices, key, __VA_ARGS__) \
  X(primitive, positions, key, __VA_ARGS__) \
  X(primitive, normals, key, __VA_ARGS__) \
  X(primitive, colors, key, __VA_ARGS__) \
  X(primitive, texCoords, key, __VA_ARGS__) \
  X(mesh, key, key, __VA_ARGS__) \
  X(mesh, primitives, key_array, __VA_ARGS__) \
  X(object, key, key, __VA_ARGS__) \
  X(object, transform, mat4, __VA_ARGS__) \
  X(object, mesh, key, __VA_ARGS__) \
  X(object, camera, key, __VA_ARGS__) \
  X(object, children, key_array, __VA_ARGS__) \
  X(camera, key, key, __VA_ARGS__) \
  X(camera, type, int, __VA_ARGS__) \
  X(camera, fovY, float, __VA_ARGS__) \
  X(camera, aspectRatio, float, __VA_ARGS__) \
  X(camera, nearZ, float, __VA_ARGS__) \
  X(camera, farZ, float, __VA_ARGS__) \
  X(scene, key, key, __VA_ARGS__) \
  X(scene, name, text, __VA_ARGS__) \
  X(scene, objects, key_array, __VA_ARGS__) \
  X(scene, cameras, key_array, __VA_ARGS__) \
  END_OF_DATA_ASSET_DB_COLUMNS
