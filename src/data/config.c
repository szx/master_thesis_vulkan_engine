#include "assets.h"

data_config *data_config_create() {
  data_config *config = core_alloc(sizeof(data_config));
  config->path = get_asset_file_path("", "config.ini");
#define alloc_int(key, ...) config->key = 0;
#define alloc_str(key, ...) utstring_new(config->key);
  DATA_CONFIG_INT_KEYS(alloc_int, )
  DATA_CONFIG_STR_KEYS(alloc_str, )
#undef alloc_int
#undef alloc_str
  config->dirty = false;
  data_config_load(config);
  return config;
}

void data_config_destroy(data_config *config) {
  utstring_free(config->path);
#define free_int(key, ...)
#define free_str(key, ...) utstring_free(config->windowTitle);
  DATA_CONFIG_INT_KEYS(free_int, )
  DATA_CONFIG_STR_KEYS(free_str, )
#undef free_str
#undef free_int
  core_free(config);
}

void data_config_load(data_config *config) {
  UT_string *s = read_text_file(config->path);
  if (utstring_len(s) == 0) {
    log_error("failed to read config file '%s'", utstring_body(config->path));
    return;
  }
  log_debug("loaded config:\n%s", utstring_body(s));
  data_config_parse(config, s);
  utstring_free(s);
}

void data_config_save(data_config *config) {
  if (!config->dirty) {
    return;
  }
  UT_string *s = data_config_get_config_str(config);
  log_debug("writing config:\n%s", utstring_body(s));
  if (!write_text_file(config->path, s)) {
    log_error("failed to write config file '%s'", utstring_body(config->path));
  }
  utstring_free(s);
  config->dirty = false;
}

void data_config_parse(data_config *config, UT_string *configStr) {
  log_debug("parsing config");
  char *str = utstring_body(configStr);
  const char *delim = "\n\r";
  char *txt;
  char *line = strtok_r(str, delim, &txt);
  // TODO: INI sections.
  while ((line = strtok_r(NULL, delim, &txt))) {
    log_trace("config line %s", line);
    size_t equalPos = 0;
    for (size_t i = 0; i < strlen(line); i++) {
      if (line[i] == '=') {
        equalPos = i;
        break;
      }
    }
    if (equalPos == 0) {
      log_error("malformed config line: %s", line);
      continue;
    }
    line[equalPos] = '\0';
    // Strip left whitespaces.
    char *key = line;
    lstrip(&key);
    rstrip(&key);
    char *value = line + equalPos + 1;
    lstrip(&value);
    rstrip(&value);
    log_debug("key='%s' value='%s'", key, value);
#define parse_int(_key, ...)                                                                       \
  if (strcmp(#_key, key) == 0)                                                                     \
    data_config_set_int(config, #_key, atoi(value));
#define parse_str(_key, ...)                                                                       \
  if (strcmp(#_key, key) == 0)                                                                     \
    data_config_set_str(config, #_key, value);
    DATA_CONFIG_INT_KEYS(parse_int, )
    DATA_CONFIG_STR_KEYS(parse_str, )
#undef parse_str
#undef parse_int
  }
  config->dirty = false;
}

UT_string *data_config_get_config_str(data_config *config) {
  UT_string *s;
  utstring_new(s);
  // TODO: INI sections.
  utstring_printf(s, "[config]\n");
#define save_int(_key, ...) utstring_printf(s, "  %s = %d \n", #_key, config->_key);
#define save_str(_key, ...) utstring_printf(s, "  %s = %s\n", #_key, utstring_body(config->_key));
  DATA_CONFIG_INT_KEYS(save_int, )
  DATA_CONFIG_STR_KEYS(save_str, )
#undef save_str
#undef save_int
  utstring_printf(s, "\n");
  return s;
}

void data_config_set_int(data_config *config, const char *key, int value) {
#define parse_int(_key, ...)                                                                       \
  if (strcmp(#_key, key) == 0) {                                                                   \
    config->_key = value;                                                                          \
    return;                                                                                        \
  }
  DATA_CONFIG_INT_KEYS(parse_int, )
#undef parse_int
  log_warn("tried to set value '%d' for unrecognized int config key '%s", value, key);
}

void data_config_set_str(data_config *config, const char *key, const char *value) {
#define parse_str(_key, ...)                                                                       \
  if (strcmp(#_key, key) == 0) {                                                                   \
    utstring_clear(config->_key);                                                                  \
    utstring_printf(config->_key, "%s", value);                                                    \
    return;                                                                                        \
  }
  DATA_CONFIG_STR_KEYS(parse_str, )
#undef parse_str
  log_error("tried to set value '%s' for unrecognized string config key '%s", value, key);
}
