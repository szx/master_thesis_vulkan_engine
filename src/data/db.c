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

#define SQLITE_PREPARE(query)                                                                      \
  do {                                                                                             \
    sqlite3_stmt *_stmt;                                                                           \
    char *_sql = query;                                                                            \
    log_debug("SQLITE_PREPARE(%s)", query);                                                        \
    int _rc = sqlite3_prepare_v2(db->db, _sql, -1, &_stmt, NULL);                                  \
    if (_rc != SQLITE_OK) {                                                                        \
      panic("database error (prepare): %s", sqlite3_errmsg(db->db));                               \
    }

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

int data_db_get_int(data_db *db, char *key) {
  int value;
  SQLITE_PREPARE("SELECT key, value FROM config WHERE key = ?;");
  sqlite3_bind_text(_stmt, 1, key, strlen(key), NULL);
  SQLITE_STEP(SQLITE_ROW);
  value = sqlite3_column_int(_stmt, 1);
  SQLITE_FINALIZE();
  return value;
}

UT_string *data_db_get_str(data_db *db, char *key) {
  UT_string *value;
  SQLITE_PREPARE("SELECT key, value FROM config WHERE key = ?;");
  sqlite3_bind_text(_stmt, 1, key, strlen(key), NULL);
  SQLITE_STEP(SQLITE_ROW);
  const unsigned char *result = sqlite3_column_text(_stmt, 1);
  utstring_new(value);
  utstring_printf(value, "%s", result);
  SQLITE_FINALIZE();
  return value;
}

int data_db_insert_int(data_db *db, char *table, char *key, char *column, int value) {
  static UT_string *sql = NULL;
  // UPSERT (UPDATE + INSERT): Insert if key doesn't exist, otherwise update.
  SQLITE_EXEC(
      "INSERT INTO %s (key, %s) VALUES(\"%s\", %d) ON CONFLICT (key) DO UPDATE SET %s = %d;", table,
      column, key, value, column, value);
  return value;
}

UT_string *data_db_insert_str(data_db *db, char *table, char *column, char *key, UT_string *value) {
  static UT_string *sql = NULL;
  SQLITE_EXEC(
      "INSERT INTO %s (key, %s) VALUES(\"%s\", \"%s\") ON CONFLICT (key) DO UPDATE SET %s = %s;",
      table, column, key, utstring_body(value), column, utstring_body(value));
  return value;
}

void data_db_insert_blob(data_db *db, char *table, char *key, char *column, void *value,
                         size_t size) {
  UT_string *query;
  utstring_new(query);
  utstring_printf(
      query, "INSERT INTO %s (key, %s) VALUES (\"%s\", ?) ON CONFLICT (key) DO UPDATE SET %s = ?;",
      table, column, key, column);
  SQLITE_PREPARE(utstring_body(query));
  sqlite3_bind_blob(_stmt, 1, value, size, NULL);
  sqlite3_bind_blob(_stmt, 2, value, size, NULL);
  SQLITE_STEP(SQLITE_DONE);
  SQLITE_FINALIZE();
  utstring_free(query);
}

void data_db_create_key_value_table_for_text(data_db *db, char *table) {
  static UT_string *sql = NULL;
  SQLITE_EXEC("DROP TABLE IF EXISTS %s;", table);
  SQLITE_EXEC("CREATE TABLE %s(key TEXT PRIMARY KEY, value TEXT);", table);
}

void data_db_create_key_value_table_for_multiple_values(data_db *db, char *table,
                                                        const char *valueNames) {
  static UT_string *sql = NULL;
  SQLITE_EXEC("DROP TABLE IF EXISTS %s;", table);
  SQLITE_EXEC("CREATE TABLE %s(key TEXT PRIMARY KEY, %s);", table, valueNames);
}
