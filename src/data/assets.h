/* SQLite database as a asset pipeline output. */
#pragma once

#include "db.h"

// TODO: generate from config
// TODO: blob support (read-only except asset pipeline?)
// TODO: meshes

// clang-format off
#define END_OF_DATA_ASSETS_TABLES
#define DATA_ASSETS_TABLES(X_text_table, X_blob_table, ...)                                         \
  X_text_table(config, __VA_ARGS__)                                                                 \
  X_blob_table(mesh, __VA_ARGS__)                                                                 \
  END_OF_DATA_ASSETS_TABLES

#define END_OF_DATA_ASSETS_KEYS
#define DATA_ASSETS_KEYS(X_int, X_string, ...)                                                     \
  X_int(config, windowWidth, __VA_ARGS__)                                                          \
  X_int(config, windowHeight, __VA_ARGS__)                                                         \
  X_string(config, windowTitle, __VA_ARGS__)                                                       \
  END_OF_DATA_ASSETS_KEYS
// clang-format on

typedef struct data_assets {
  data_db *db; /// SQLite database file.
  /* config */
  int windowWidth;
  int windowHeight;
  UT_string *windowTitle;
} data_assets;

data_assets *data_assets_create(bool shouldLoad);
void data_assets_destroy(data_assets *assets);
/// Saves config.
void data_assets_save(data_assets *assets);
/// Saves default config.
void data_assets_save_default(data_assets *assets);
