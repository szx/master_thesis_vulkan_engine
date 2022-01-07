#include "db.h"

data_db *data_db_create(UT_string *path) {
  data_db *db = core_alloc(sizeof(data_db));
  utstring_new(db->path);
  utstring_concat(db->path, path);
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

#define SQLITE_PREPARE(query)                                                                      \
  do {                                                                                             \
    sqlite3_stmt *stmt;                                                                            \
    char *sql = query;                                                                             \
    int rc = sqlite3_prepare_v2(db->db, sql, -1, &stmt, NULL);                                     \
    if (rc != SQLITE_OK) {                                                                         \
      panic("database error (prepare): %s", sqlite3_errmsg(db->db));                               \
    }

#define SQLITE_STEP()                                                                              \
  rc = sqlite3_step(stmt);                                                                         \
  if (rc != SQLITE_ROW) {                                                                          \
    panic("database error (step): %s", sqlite3_errmsg(db->db));                                    \
  }

#define SQLITE_FINALIZE()                                                                          \
  sqlite3_finalize(stmt);                                                                          \
  }                                                                                                \
  while (0)

#define SQLITE_EXEC(query, ...)                                                                    \
  {                                                                                                \
    utstring_renew(sql);                                                                           \
    utstring_printf(sql, query, __VA_ARGS__);                                                      \
    int rc = sqlite3_exec(db->db, utstring_body(sql), NULL, NULL, NULL);                           \
    if (rc != SQLITE_OK) {                                                                         \
      panic("database error (exec): %s", sqlite3_errmsg(db->db));                                  \
    }                                                                                              \
  }

int data_db_get_int(data_db *db, char *key) {
  int value;
  SQLITE_PREPARE("SELECT key, value FROM config WHERE key = ?;");
  sqlite3_bind_text(stmt, 1, key, strlen(key), NULL);
  SQLITE_STEP();
  value = sqlite3_column_int(stmt, 1);
  SQLITE_FINALIZE();
  return value;
}

UT_string *data_db_get_str(data_db *db, char *key) {
  UT_string *value;
  SQLITE_PREPARE("SELECT key, value FROM config WHERE key = ?;");
  sqlite3_bind_text(stmt, 1, key, strlen(key), NULL);
  SQLITE_STEP();
  const unsigned char *result = sqlite3_column_text(stmt, 1);
  utstring_new(value);
  utstring_printf(value, "%s", result);
  SQLITE_FINALIZE();
  return value;
}

int data_db_insert_int(data_db *db, char *table, char *key, int value) {
  static UT_string *sql = NULL;
  SQLITE_EXEC("INSERT OR REPLACE INTO %s VALUES(\"%s\", %d);", table, key, value);
  return value;
}

UT_string *data_db_insert_str(data_db *db, char *table, char *key, UT_string *value) {
  static UT_string *sql = NULL;
  SQLITE_EXEC("INSERT OR REPLACE INTO %s VALUES(\"%s\", \"%s\");", table, key,
              utstring_body(value));
  return value;
}

void data_db_create_key_value_table_for_text(data_db *db, char *table) {
  static UT_string *sql = NULL;
  SQLITE_EXEC("DROP TABLE IF EXISTS %s;", table);
  SQLITE_EXEC("CREATE TABLE %s(key TEXT PRIMARY KEY, value TEXT);", table);
}

void data_db_create_key_value_table_for_blobs(data_db *db, char *table) {
  static UT_string *sql = NULL;
  SQLITE_EXEC("DROP TABLE IF EXISTS %s;", table);
  SQLITE_EXEC("CREATE TABLE %s(key TEXT PRIMARY KEY, value BLOB);", table);
}
