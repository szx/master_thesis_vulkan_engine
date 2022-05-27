/* This file was autogenerated. */

#pragma once

#define END_OF_DATA_ASSET_CONFIG_SECTION
#define DATA_ASSET_CONFIG_SECTIONS(X, ...) \
  X(controls, __VA_ARGS__) \
  X(settings, __VA_ARGS__) \
  X(graphics, __VA_ARGS__) \
  END_OF_DATA_ASSET_CONFIG_SECTION

#define END_OF_DATA_ASSET_CONFIG_INT_KEYS
#define DATA_ASSET_CONFIG_INT_KEYS(X, ...) \
  X(controls, Enabled, 1, __VA_ARGS__) \
  X(graphics, WindowWidth, 640, __VA_ARGS__) \
  X(graphics, WindowHeight, 480, __VA_ARGS__) \
  X(graphics, EnabledInstancing, 1, __VA_ARGS__) \
  X(graphics, MaxPrimitiveElementCount, 1000, __VA_ARGS__) \
  END_OF_DATA_ASSET_CONFIG_INT_KEYS

#define END_OF_DATA_ASSET_CONFIG_STR_KEYS
#define DATA_ASSET_CONFIG_STR_KEYS(X, ...) \
  X(settings, StartScene, "MetalRoughSpheresNoTextures", __VA_ARGS__) \
  X(graphics, WindowTitle, "cpptest", __VA_ARGS__) \
  X(graphics, Font, "Go-Mono", __VA_ARGS__) \
  END_OF_DATA_ASSET_CONFIG_STR_KEYS

#define END_OF_DATA_SCENE_CONFIG_SECTION
#define DATA_SCENE_CONFIG_SECTIONS(X, ...) \
  X(skybox, __VA_ARGS__) \
  END_OF_DATA_SCENE_CONFIG_SECTION

#define END_OF_DATA_SCENE_CONFIG_INT_KEYS
#define DATA_SCENE_CONFIG_INT_KEYS(X, ...) \
  X(skybox, AmbientIntensity, 0, __VA_ARGS__) \
  END_OF_DATA_SCENE_CONFIG_INT_KEYS

#define END_OF_DATA_SCENE_CONFIG_STR_KEYS
#define DATA_SCENE_CONFIG_STR_KEYS(X, ...) \
  X(skybox, Name, "skybox1", __VA_ARGS__) \
  END_OF_DATA_SCENE_CONFIG_STR_KEYS

#define END_OF_DATA_CONFIG_SCHEMA
#define DATA_CONFIG_SCHEMA(X, ...) \
  X(ASSET, asset, __VA_ARGS__) \
  X(SCENE, scene, __VA_ARGS__) \
  END_OF_DATA_CONFIG_SCHEMA

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
  X(image, key, key, width, int, height, int, depth, int, channels, int, facecount, int, type, int, data, byte_array, __VA_ARGS__) \
  X(sampler, key, key, magfilter, int, minfilter, int, addresswrapu, int, addresswrapv, int, __VA_ARGS__) \
  X(texture, key, key, image, key, sampler, key, __VA_ARGS__) \
  X(material, key, key, basecolorfactor, vec4, metallicfactor, float, roughnessfactor, float, basecolortexture, key, metallicroughnesstexture, key, normalmaptexture, key, __VA_ARGS__) \
  X(vertexAttribute, key, key, valuesint, int_array, valuesvec2, vec2_array, valuesvec3, vec3_array, valuesvec4, vec4_array, __VA_ARGS__) \
  X(primitive, key, key, material, key, topology, int, indices, key, positions, key, normals, key, colors, key, texcoords, key, tangents, key, __VA_ARGS__) \
  X(mesh, key, key, primitives, key_array, __VA_ARGS__) \
  X(object, key, key, transform, mat4, mesh, key, camera, key, children, key_array, __VA_ARGS__) \
  X(camera, key, key, type, int, fovy, float, aspectratio, float, nearz, float, farz, float, __VA_ARGS__) \
  X(directLight, key, key, type, int, position, vec3, innerconeangle, float, outerconeangle, float, intensity, float, range, float, color, vec3, __VA_ARGS__) \
  X(skybox, key, key, name, text, cubemaptexture, key, __VA_ARGS__) \
  X(font, key, key, name, text, fonttexture, key, characters, text, charactersize, int, __VA_ARGS__) \
  X(scene, key, key, name, text, objects, key_array, directlights, key_array, skybox, key, __VA_ARGS__) \
  END_OF_DATA_ASSET_DB_TABLES

#define END_OF_DATA_ASSET_DB_COLUMNS
#define DATA_ASSET_DB_COLUMNS(X, ...) \
  X(image, key, key, __VA_ARGS__) \
  X(image, width, int, __VA_ARGS__) \
  X(image, height, int, __VA_ARGS__) \
  X(image, depth, int, __VA_ARGS__) \
  X(image, channels, int, __VA_ARGS__) \
  X(image, faceCount, int, __VA_ARGS__) \
  X(image, type, int, __VA_ARGS__) \
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
  X(material, normalMapTexture, key, __VA_ARGS__) \
  X(vertexAttribute, key, key, __VA_ARGS__) \
  X(vertexAttribute, valuesInt, int_array, __VA_ARGS__) \
  X(vertexAttribute, valuesVec2, vec2_array, __VA_ARGS__) \
  X(vertexAttribute, valuesVec3, vec3_array, __VA_ARGS__) \
  X(vertexAttribute, valuesVec4, vec4_array, __VA_ARGS__) \
  X(primitive, key, key, __VA_ARGS__) \
  X(primitive, material, key, __VA_ARGS__) \
  X(primitive, topology, int, __VA_ARGS__) \
  X(primitive, indices, key, __VA_ARGS__) \
  X(primitive, positions, key, __VA_ARGS__) \
  X(primitive, normals, key, __VA_ARGS__) \
  X(primitive, colors, key, __VA_ARGS__) \
  X(primitive, texCoords, key, __VA_ARGS__) \
  X(primitive, tangents, key, __VA_ARGS__) \
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
  X(directLight, key, key, __VA_ARGS__) \
  X(directLight, type, int, __VA_ARGS__) \
  X(directLight, position, vec3, __VA_ARGS__) \
  X(directLight, innerConeAngle, float, __VA_ARGS__) \
  X(directLight, outerConeAngle, float, __VA_ARGS__) \
  X(directLight, intensity, float, __VA_ARGS__) \
  X(directLight, range, float, __VA_ARGS__) \
  X(directLight, color, vec3, __VA_ARGS__) \
  X(skybox, key, key, __VA_ARGS__) \
  X(skybox, name, text, __VA_ARGS__) \
  X(skybox, cubemapTexture, key, __VA_ARGS__) \
  X(font, key, key, __VA_ARGS__) \
  X(font, name, text, __VA_ARGS__) \
  X(font, fontTexture, key, __VA_ARGS__) \
  X(font, characters, text, __VA_ARGS__) \
  X(font, characterSize, int, __VA_ARGS__) \
  X(scene, key, key, __VA_ARGS__) \
  X(scene, name, text, __VA_ARGS__) \
  X(scene, objects, key_array, __VA_ARGS__) \
  X(scene, directLights, key_array, __VA_ARGS__) \
  X(scene, skybox, key, __VA_ARGS__) \
  END_OF_DATA_ASSET_DB_COLUMNS
