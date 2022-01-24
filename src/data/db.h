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

/* supported data types */

typedef uint64_t data_int;
typedef UT_string *data_text;
typedef struct data_blob {
  void *memory;
  size_t size;
} data_blob;

#define data_blob_alloc_memory(_blob, _size)                                                       \
  do {                                                                                             \
    (_blob).size = _size;                                                                          \
    (_blob).memory = core_alloc((_blob).size);                                                     \
  } while (0)
#define data_blob_free_memory(_blob) core_free((_blob).memory)

#define utarray_blob(_array)                                                                       \
  (data_blob) { utarray_front(_array), utarray_size(_array) }
#define utstring_blob(_s)                                                                          \
  (data_blob) { utstring_body(_s), utstring_len(_s) }

typedef hash_t data_hash;
#define hash_blob(_hash)                                                                           \
  (data_blob) { &(_hash), sizeof(_hash) }

typedef data_blob data_hash_array;
#define utarray_hash_array(_array) utarray_blob(_array)
UT_array *hash_array_utarray(data_hash_array hashArray);

typedef struct data_mat4 {
  mat4 value; // In C functions can not return arrays, so we wrap it in struct.
} data_mat4;

/* select */

data_int data_db_select_int(data_db *db, char *table, data_blob key, char *column);

data_text data_db_select_text(data_db *db, char *table, data_blob key, char *column);

data_blob data_db_select_blob(data_db *db, char *table, data_blob key, char *column);

data_hash data_db_select_hash(data_db *db, char *table, data_blob key, char *column);

data_hash_array data_db_select_hash_array(data_db *db, char *table, data_blob key, char *column);

data_mat4 data_db_select_mat4(data_db *db, char *table, data_blob key, char *column);

/* insert */

void data_db_insert_int(data_db *db, char *table, data_blob key, char *column, data_int value,
                        bool updateIfExists);

void data_db_insert_text(data_db *db, char *table, data_blob key, char *column, data_text value,
                         bool updateIfExists);

void data_db_insert_blob(data_db *db, char *table, data_blob key, char *column, data_blob value,
                         bool updateIfExists);

void data_db_insert_hash(data_db *db, char *table, data_blob key, char *column, data_hash value,
                         bool updateIfExists);

void data_db_insert_hash_array(data_db *db, char *table, data_blob key, char *column,
                               data_hash_array value, bool updateIfExists);

void data_db_insert_mat4(data_db *db, char *table, data_blob key, char *column, data_mat4 value,
                         bool updateIfExists);

/* create */

/// Creates table with key/value (text/blobs) schema.
/// NOTE: SQLite has "dynamic type system", which means that we can store
///       integers in text fields.
void data_db_create_key_value_table_for_multiple_values(data_db *db, char *table,
                                                        const char *keyName, const char *keyType,
                                                        const char *columnDefs[],
                                                        size_t columnDefsCount);
