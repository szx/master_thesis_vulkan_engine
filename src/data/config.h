/* Config with parser for INI configuration files.
 * Supports int and string keys.
 */

#pragma once

#include "../codegen/macros.h"

/// Key-value config.
/// It corresponds to multiple sections of INI file.
/// FIXME: Implementation is case sensitive.
typedef struct data_config {
  UT_string *path; /// INI config filepath.
  /* state */
#define decl_int(_section, _key, ...) int _section##_key;
#define decl_str(_section, _key, ...) UT_string *_section##_key;
  DATA_CONFIG_INT_KEYS(decl_int, decl_empty, )
  DATA_CONFIG_STR_KEYS(decl_str, decl_empty, )
#undef decl_int
#undef decl_str
  bool dirty; // True if config is updated.
} data_config;

data_config *data_config_create();
void data_config_destroy(data_config *config);
/// Reads config state from disk and tries to parse.
void data_config_load(data_config *config);
/// Writes config state to disk.
void data_config_save(data_config *config);
/// Parses string and updates config state.
/// NOTE: Can replace newlines and '=' in configStr with '\0'.
void data_config_parse(data_config *config, UT_string *configStr);
/// Returns default config string representation.
UT_string *data_config_get_config_str(data_config *config);

void data_config_set_int(data_config *config, const char *section, const char *key, int value);
void data_config_set_str(data_config *config, const char *section, const char *key,
                         const char *value);
void data_config_set_default_int(data_config *config, const char *section, const char *key);
void data_config_set_default_str(data_config *config, const char *section, const char *key);
void data_config_set_default_values(data_config *config);
