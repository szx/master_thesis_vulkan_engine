/* SQLite database as a asset database. */
#pragma once

#include "config.h"
#include "db.h"

// clang-format off
#define END_OF_DATA_ASSETS_BLOB_TABLES
#define DATA_ASSETS_BLOB_TABLES(X, ...)                                        \
  X(mesh, __VA_ARGS__)                                                                  \
  END_OF_DATA_ASSETS_BLOB_TABLES
// clang-format on

typedef struct data_assets {
  data_db *db;     /// SQLite database file.
  UT_string *path; /// SQLite database filepath.
  /* config */
  data_config *config;
  // TODO: Fetching mesh blobs to CPU.
} data_assets;

data_assets *data_assets_create();
void data_assets_destroy(data_assets *assets);
/// Saves assets database.
void data_assets_save(data_assets *assets);
/// Saves empty assets database.
/// Used by asset pipeline.
void data_assets_save_empty(data_assets *assets);
