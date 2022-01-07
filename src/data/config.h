/* SQLite database as asset pipeline output. */
#pragma once

#include "../core/platform.h"

#include <sqlite3.h>

typedef struct data_config {
  sqlite3 *db; // database connection handle

  // loaded config:
  int windowWidth;
  int windowHeight;
  UT_string *windowTitle;
} data_config;

// Returns new config.
// Opens new connection to assets/data.db.
data_config data_config_init();

// Frees config.
// Closes connection to assets/data.db.
void data_config_free(data_config *config);

// Returns int value for text key.
int data_config_get_int(data_config *config, char *key);

// Returns text value for text key.
UT_string *data_config_get_str(data_config *config, char *key);
