/* SQLite database helpers. */
#pragma once

#include "../core/core.h"

/// SQLite database file.
typedef struct data_db {
  UT_string *path;
  sqlite3 *db;
} data_db;

data_db *data_db_create(UT_string *path);
void data_db_destroy(data_db *db);

/* select */

/// Returns int value of a text key.
int data_db_get_int(data_db *db, char *key);

/// Returns new string value of a text key.
UT_string *data_db_get_str(data_db *db, char *key);

/* insert */

typedef int data_int;
typedef UT_string *data_str;
typedef struct data_blob {
  void *memory;
  size_t size;
} data_blob;
#define utarray_blob(array)                                                                        \
  (data_blob) { utarray_front(array), utarray_size(array) }

/// Insertes int value for a blob key.
void data_db_insert_int(data_db *db, char *table, void *key, size_t keySize, char *column,
                        data_int value, bool updateIfExists);

/// Inserts string value for a blob key.
void data_db_insert_str(data_db *db, char *table, void *key, size_t keySize, char *column,
                        data_str value, bool updateIfExists);

/// Inserts blob for a blob key.
void data_db_insert_blob(data_db *db, char *table, void *key, size_t keySize, char *column,
                         data_blob value, bool updateIfExists);

/* create */

/// Creates table with key/value (text/blobs) schema.
/// NOTE: SQLite has "dynamic type system", which means that we can store
///       integers in text fields.
void data_db_create_key_value_table_for_multiple_values(data_db *db, char *table,
                                                        const char *keyDef, const char *valueDef);
