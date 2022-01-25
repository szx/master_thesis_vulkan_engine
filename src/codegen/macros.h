/* This file was autogenerated. */

#pragma once

#define END_OF_DATA_CONFIG_SECTION
#define DATA_CONFIG_SECTIONS(X, ...) \
  X(graphics, __VA_ARGS__) \
  X(controls, __VA_ARGS__) \
  X(settings, __VA_ARGS__) \
  END_OF_DATA_CONFIG_SECTION

#define END_OF_DATA_CONFIG_INT_KEYS
#define DATA_CONFIG_INT_KEYS(X, ...) \
  X(graphics, WindowWidth, 640, __VA_ARGS__) \
  X(graphics, WindowHeight, 480, __VA_ARGS__) \
  X(controls, Enabled, 1, __VA_ARGS__) \
  END_OF_DATA_CONFIG_INT_KEYS

#define END_OF_DATA_CONFIG_STR_KEYS
#define DATA_CONFIG_STR_KEYS(X, ...) \
  X(graphics, WindowTitle, "cpptext", __VA_ARGS__) \
  X(settings, StartScene, "triangles", __VA_ARGS__) \
  END_OF_DATA_CONFIG_STR_KEYS

#define END_OF_DATA_ASSET_DB_TABLES
#define DATA_ASSET_DB_TABLES(X, ...) \
  X(primitive, key, hash, topology, int, indices, blob, positions, blob, normals, blob, colors, blob, texcoords, blob, __VA_ARGS__) \
  X(mesh, key, hash, primitives, hash_array, __VA_ARGS__) \
  X(node, key, hash, transform, mat4, nodes, blob, mesh, hash, __VA_ARGS__) \
  X(scene, key, text, nodes, hash_array, cameras, blob, __VA_ARGS__) \
  END_OF_DATA_ASSET_DB_TABLES

#define END_OF_DATA_ASSET_DB_COLUMNS
#define DATA_ASSET_DB_COLUMNS(X, ...) \
  X(primitive, key, hash, __VA_ARGS__) \
  X(primitive, topology, int, __VA_ARGS__) \
  X(primitive, indices, blob, __VA_ARGS__) \
  X(primitive, positions, blob, __VA_ARGS__) \
  X(primitive, normals, blob, __VA_ARGS__) \
  X(primitive, colors, blob, __VA_ARGS__) \
  X(primitive, texCoords, blob, __VA_ARGS__) \
  X(mesh, key, hash, __VA_ARGS__) \
  X(mesh, primitives, hash_array, __VA_ARGS__) \
  X(node, key, hash, __VA_ARGS__) \
  X(node, transform, mat4, __VA_ARGS__) \
  X(node, nodes, blob, __VA_ARGS__) \
  X(node, mesh, hash, __VA_ARGS__) \
  X(scene, key, text, __VA_ARGS__) \
  X(scene, nodes, hash_array, __VA_ARGS__) \
  X(scene, cameras, blob, __VA_ARGS__) \
  END_OF_DATA_ASSET_DB_COLUMNS
