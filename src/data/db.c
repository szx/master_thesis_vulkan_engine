#include "db.h"

data_db *data_db_create(UT_string *path) {
  data_db *db = core_alloc(sizeof(data_db));
  utstring_new(db->path);
  utstring_concat(db->path, path);
  log_debug("opening SQLite database '%s'", utstring_body(db->path));
  int rc = sqlite3_open(utstring_body(db->path), &db->db);
  if (rc != SQLITE_OK) {
    panic("database error (1): %s", sqlite3_errmsg(db->db));
  }
  return db;
}

void data_db_destroy(data_db *db) {
  utstring_free(db->path);
  sqlite3_close(db->db);
  core_free(db);
}

#define SQLITE_PREPARE(query, ...)                                                                 \
  do {                                                                                             \
    sqlite3_stmt *_stmt;                                                                           \
    UT_string *_sql;                                                                               \
    utstring_new(_sql);                                                                            \
    utstring_printf(_sql, query, __VA_ARGS__);                                                     \
    log_debug("SQLITE_PREPARE(%s)", utstring_body(_sql));                                          \
    int _rc = sqlite3_prepare_v2(db->db, utstring_body(_sql), -1, &_stmt, NULL);                   \
    if (_rc != SQLITE_OK) {                                                                        \
      panic("database error (prepare): %s", sqlite3_errmsg(db->db));                               \
    }                                                                                              \
    utstring_free(_sql);

#define SQLITE_STEP(expected)                                                                      \
  _rc = sqlite3_step(_stmt);                                                                       \
  if (_rc != expected) {                                                                           \
    panic("database error (step): %s", sqlite3_errmsg(db->db));                                    \
  }

#define SQLITE_FINALIZE()                                                                          \
  sqlite3_finalize(_stmt);                                                                         \
  }                                                                                                \
  while (0)

#define SQLITE_EXEC(query, ...)                                                                    \
  {                                                                                                \
    utstring_renew(sql);                                                                           \
    utstring_printf(sql, query, __VA_ARGS__);                                                      \
    log_debug("SQLITE_EXEC(%s)", utstring_body(sql));                                              \
    int rc = sqlite3_exec(db->db, utstring_body(sql), NULL, NULL, NULL);                           \
    if (rc != SQLITE_OK) {                                                                         \
      panic("database error (exec): %s", sqlite3_errmsg(db->db));                                  \
    }                                                                                              \
  }

/* select */

data_int data_db_select_int(data_db *db, char *table, data_blob key, char *column) {
  int value = 0;
  SQLITE_PREPARE("SELECT key, %s FROM %s WHERE key = ?;", column, table);
  sqlite3_bind_blob(_stmt, 1, key.memory, key.size, NULL);
  SQLITE_STEP(SQLITE_ROW);
  value = sqlite3_column_int(_stmt, 1);
  SQLITE_FINALIZE();
  return value;
}

data_str data_db_select_str(data_db *db, char *table, data_blob key, char *column) {
  UT_string *value;
  utstring_new(value);
  SQLITE_PREPARE("SELECT key, %s FROM %s WHERE key = ?;", column, table);
  sqlite3_bind_blob(_stmt, 1, key.memory, key.size, NULL);
  SQLITE_STEP(SQLITE_ROW);
  const unsigned char *result = sqlite3_column_text(_stmt, 1);
  utstring_printf(value, "%s", result);
  SQLITE_FINALIZE();
  return value;
}

data_blob data_db_select_blob(data_db *db, char *table, data_blob key, char *column) {
  data_blob value;
  SQLITE_PREPARE("SELECT key, %s FROM %s WHERE key = ?;", column, table);
  sqlite3_bind_blob(_stmt, 1, key.memory, key.size, NULL);
  SQLITE_STEP(SQLITE_ROW);
  const void *memory = sqlite3_column_blob(_stmt, 1);
  size_t size = sqlite3_column_bytes(_stmt, 1);
  value.size = size;
  value.memory = core_memdup(memory, value.size);
  SQLITE_FINALIZE();
  return value;
}

data_hash data_db_select_hash(data_db *db, char *table, data_blob key, char *column) {
  data_blob blob = data_db_select_blob(db, table, key, column);
  data_hash hash = *(data_hash *)blob.memory;
  core_free(blob.memory);
  return hash;
}

data_hash_array data_db_select_hash_array(data_db *db, char *table, data_blob key, char *column) {
  return data_db_select_blob(db, table, key, column);
}

/* insert */

void data_db_insert_int(data_db *db, char *table, data_blob key, char *column, data_int value,
                        bool updateIfExists) {
  const char *query;
  if (updateIfExists) {
    query = "INSERT INTO %s (key, %s) VALUES (?, %d) ON CONFLICT (key) DO UPDATE SET %s = %d;";
  } else {
    query = "INSERT INTO %s (key, %s) VALUES (?, %d) ON CONFLICT (key) DO NOTHING;";
  }
  SQLITE_PREPARE(query, table, column, value, column, value);
  sqlite3_bind_blob(_stmt, 1, key.memory, key.size, NULL);
  SQLITE_STEP(SQLITE_DONE);
  SQLITE_FINALIZE();
}

void data_db_insert_str(data_db *db, char *table, data_blob key, char *column, data_str value,
                        bool updateIfExists) {
  const char *query;
  if (updateIfExists) {
    query =
        "INSERT INTO %s (key, %s) VALUES (?, \"%s\") ON CONFLICT (key) DO UPDATE SET %s = \"%s\";";
  } else {
    query = "INSERT INTO %s (key, %s) VALUES (?, \"%s\") ON CONFLICT (key) DO NOTHING;";
  }
  SQLITE_PREPARE(query, table, column, utstring_body(value), column, utstring_body(value));
  sqlite3_bind_blob(_stmt, 1, key.memory, key.size, NULL);
  SQLITE_STEP(SQLITE_DONE);
  SQLITE_FINALIZE();
}

void data_db_insert_blob(data_db *db, char *table, data_blob key, char *column, data_blob value,
                         bool updateIfExists) {
  const char *query;
  if (updateIfExists) {
    query = "INSERT INTO %s (key, %s) VALUES (?, ?) ON CONFLICT (key) DO UPDATE SET %s = ?;";
  } else {
    query = "INSERT INTO %s (key, %s) VALUES (?, ?) ON CONFLICT (key) DO NOTHING;";
  }
  SQLITE_PREPARE(query, table, column, column);
  sqlite3_bind_blob(_stmt, 1, key.memory, key.size, NULL);
  sqlite3_bind_blob(_stmt, 2, value.memory, value.size, NULL);
  if (updateIfExists) {
    sqlite3_bind_blob(_stmt, 3, value.memory, value.size, NULL);
  }
  SQLITE_STEP(SQLITE_DONE);
  SQLITE_FINALIZE();
}

void data_db_insert_hash(data_db *db, char *table, data_blob key, char *column, data_hash value,
                         bool updateIfExists) {
  data_db_insert_blob(db, table, key, column, (data_blob){&value, sizeof(value)}, updateIfExists);
}

void data_db_insert_hash_array(data_db *db, char *table, data_blob key, char *column,
                               data_hash_array value, bool updateIfExists) {
  data_db_insert_blob(db, table, key, column, value, updateIfExists);
}

void data_db_create_key_value_table_for_multiple_values(data_db *db, char *table,
                                                        const char *keyDef,
                                                        const char *columnDefs[],
                                                        size_t columnDefsCount) {
  static UT_string *sql = NULL;
  SQLITE_EXEC("DROP TABLE IF EXISTS %s;", table);
  UT_string *columns;
  utstring_new(columns);
  for (size_t i = 0; i < columnDefsCount; i += 2) {
    const char *name = columnDefs[i];
    const char *type = columnDefs[i + 1];
    if (strcmp(type, "HASH") == 0 || strcmp(type, "HASH_ARRAY") == 0) {
      // Hash is just a blob with special *_get() and *_insert().
      type = "BLOB";
    }
    utstring_printf(columns, "%s %s%s", name, type, (i + 2 < columnDefsCount ? "," : ""));
  }
  SQLITE_EXEC("CREATE TABLE %s(%s PRIMARY KEY, %s);", table, keyDef, utstring_body(columns));
  utstring_free(columns);
}
