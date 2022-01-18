/* SQLite database as a asset database. */
#pragma once

#include "config.h"
#include "db.h"

#define END_OF_DATA_ASSETS_TABLES
#define DATA_ASSETS_TABLES(X, ...)                                                                 \
  /*X(mesh, "primitives BLOB", __VA_ARGS__)*/                                                      \
  X(primitive, "topology INT, indices BLOB, vertices BLOB, vertexAttributes INT", __VA_ARGS__)     \
  END_OF_DATA_ASSETS_TABLES

typedef struct data_assets {
  data_db *db;     /// SQLite database file.
  UT_string *path; /// SQLite database filepath.
  // TODO: Fetching mesh blobs to CPU.
} data_assets;

data_assets *data_assets_create();
void data_assets_destroy(data_assets *assets);
/// Saves assets database.
void data_assets_save(data_assets *assets);
/// Saves empty assets database.
/// Used by asset pipeline.
void data_assets_save_empty(data_assets *assets);

#define decl_table(table, value_names, ...)                                                        \
  void data_assets_insert_##table##_int(data_assets *assets, char *key, char *column, int value);  \
  void data_assets_insert_##table##_blob(data_assets *assets, char *key, char *column,             \
                                         void *value, size_t size);
DATA_ASSETS_TABLES(decl_table, )
#undef decl_table
