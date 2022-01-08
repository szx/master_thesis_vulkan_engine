/* Config with parser for INI configuration files.
 * Supports int and string keys.
 */

#pragma once

// clang-format off
// Note: Further [config] key definitions should be added here:

// TODO: INI sections
#define END_OF_DATA_CONFIG_INT_KEYS
#define DATA_CONFIG_INT_KEYS(X, ...)                                                     \
  X(windowWidth, __VA_ARGS__)                                                          \
  X(windowHeight, __VA_ARGS__)                                                         \
  END_OF_DATA_CONFIG_INT_KEYS

#define END_OF_DATA_CONFIG_STR_KEYS
#define DATA_CONFIG_STR_KEYS(X, ...)                                                     \
  X(windowTitle, __VA_ARGS__)                                                       \
  END_OF_DATA_CONFIG_STR_KEYS
// clang-format on

/// Key-value config.
/// It corresponds to one section of INI file.
/// INI file with multiple sections requires multiple data_config instances.
typedef struct data_config {
  UT_string *path; /// INI config filepath.
  /* state */
#define decl_int(key, ...) int key;
#define decl_str(key, ...) UT_string *key;
  DATA_CONFIG_INT_KEYS(decl_int, )
  DATA_CONFIG_STR_KEYS(decl_str, )
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
/// Note: Can replace newlines and '=' in configStr with '\0'.
void data_config_parse(data_config *config, UT_string *configStr);
/// Returns default config string representation.
UT_string *data_config_get_config_str(data_config *config);
/// Sets int value for a config key.
void data_config_set_int(data_config *config, const char *key, int value);
/// Sets copied string value for a config key.
void data_config_set_str(data_config *config, const char *key, const char *value);
