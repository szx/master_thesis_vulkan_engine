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
#define utarray_blob(_array)                                                                       \
  (data_blob) { utarray_front(_array), utarray_size(_array) }
#define utstring_blob(_s)                                                                          \
  (data_blob) { utstring_body(_s), utstring_len(_s) }
typedef hash_t data_hash;
#define hash_blob(_hash)                                                                           \
  (data_blob) { &(_hash), sizeof(_hash) }
typedef data_blob data_hash_array;
#define utarray_hash_array(_array) utarray_blob(_array)

void data_db_insert_int(data_db *db, char *table, data_blob key, char *column, data_int value,
                        bool updateIfExists);

void data_db_insert_str(data_db *db, char *table, data_blob key, char *column, data_str value,
                        bool updateIfExists);

void data_db_insert_blob(data_db *db, char *table, data_blob key, char *column, data_blob value,
                         bool updateIfExists);

void data_db_insert_hash(data_db *db, char *table, data_blob key, char *column, data_hash value,
                         bool updateIfExists);

void data_db_insert_hash_array(data_db *db, char *table, data_blob key, char *column,
                               data_hash_array value, bool updateIfExists);

/* create */

/// Creates table with key/value (text/blobs) schema.
/// NOTE: SQLite has "dynamic type system", which means that we can store
///       integers in text fields.
void data_db_create_key_value_table_for_multiple_values(data_db *db, char *table,
                                                        const char *keyDef,
                                                        const char *columnDefs[],
                                                        size_t columnDefsCount);
