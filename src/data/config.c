#include "config.h"

data_config *data_config_create(UT_string *path, data_config_type type) {
  data_config *config = core_alloc(sizeof(data_config));
  config->path = path;
  config->type = type;
#define alloc_int(_section, _key, _, _name) config->_name._section##_key = 0;
#define alloc_str(_section, _key, _, _name) utstring_new(config->_name._section##_key);
#define x(_macro, _name, ...)                                                                      \
  DATA_##_macro##_CONFIG_INT_KEYS(alloc_int, _name)                                                \
      DATA_##_macro##_CONFIG_STR_KEYS(alloc_str, _name)
  DATA_CONFIG_SCHEMA(x, )
#undef x
#undef alloc_int
#undef alloc_str
  config->dirty = false;
  data_config_set_default_values(config);
  data_config_load(config);
  return config;
}

void data_config_destroy(data_config *config) {
#define free_str(_section, _key, _, _name) utstring_free(config->_name._section##_key);
#define x(_macro, _name, ...) DATA_##_macro##_CONFIG_STR_KEYS(free_str, _name)
  DATA_CONFIG_SCHEMA(x, )
#undef x
#undef free_str
  core_free(config);
}

void data_config_load(data_config *config) {
  UT_string *s = read_text_file(config->path);
  if (utstring_len(s) == 0) {
    log_error("failed to read config file '%s'", utstring_body(config->path));
    utstring_free(s);
    return;
  }
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
  char *str = utstring_body(configStr);
  const char *delim = "\n\r";
  char *txt;
  char *line = strtok_r(str, delim, &txt);
  char *section = NULL;
  while (line != NULL) {
    strstrip(&line);
    if (strlen(line) < 3) {
      log_error("invalid config line '%s'", line);
      return;
    }
    // parse [section]
    if (line[0] == '[') {
      section = line + 1;
      section[strlen(section) - 1] = '\0';
      goto next_line;
    }
    if (section == NULL) {
      log_error("invalid config line without section '%s'", line);
      return;
    }

    // parse key = value
    size_t equalPos = 0;
    for (size_t i = 0; i < strlen(line); i++) {
      if (line[i] == '=') {
        equalPos = i;
        break;
      }
    }
    if (equalPos == 0) {
      log_error("invalid config line '%s'", line);
      continue;
    }
    line[equalPos] = '\0';
    char *key = line;
    strstrip(&key);
    char *value = line + equalPos + 1;
    strstrip(&value);

    // set config key with value
#define parse_int(_section, _key, _, _name)                                                        \
  if (strcmp(#_section, section) == 0 && strcmp(#_key, key) == 0)                                  \
    data_config_set_int(config, #_section, #_key, atoi(value));
#define parse_str(_section, _key, _, _name)                                                        \
  if (strcmp(#_section, section) == 0 && strcmp(#_key, key) == 0)                                  \
    data_config_set_str(config, #_section, #_key, value);
#define x(_macro, _name, ...)                                                                      \
  if (config->type == data_config_type_##_name) {                                                  \
    DATA_##_macro##_CONFIG_INT_KEYS(parse_int, ) DATA_##_macro##_CONFIG_STR_KEYS(parse_str, )      \
  }
    DATA_CONFIG_SCHEMA(x, )
#undef x
#undef parse_str
#undef parse_int

  next_line:
    line = strtok_r(NULL, delim, &txt);
  }
  config->dirty = false;
}

UT_string *data_config_get_config_str(data_config *config) {
  UT_string *s;
  utstring_new(s);

#define section_num(_section, ...) +1
#define section_str(_section, ...) #_section,
  size_t configSectionCount = 0;
  const char **configSections = NULL;
#define x(_macro, _name, ...)                                                                      \
  static const size_t _name##ConfigSectionCount =                                                  \
      0 DATA_##_macro##_CONFIG_SECTIONS(section_num, );                                            \
  static const char *_name##ConfigSections[] = {DATA_##_macro##_CONFIG_SECTIONS(section_str, )};   \
  if (config->type == data_config_type_##_name) {                                                  \
    configSectionCount = _name##ConfigSectionCount;                                                \
    configSections = _name##ConfigSections;                                                        \
  }
  DATA_CONFIG_SCHEMA(x, )
#undef x
#undef section_str
#undef section_num

  for (size_t i = 0; i < configSectionCount; i++) {
    const char *section = configSections[i];
    utstring_printf(s, "[%s]\n", section);
#define save_int(_section, _key, _, _name)                                                         \
  if (strcmp(#_section, section) == 0) {                                                           \
    utstring_printf(s, "  %s = %d \n", #_key, config->_name._section##_key);                       \
  }
#define save_str(_section, _key, _, _name)                                                         \
  if (strcmp(#_section, section) == 0) {                                                           \
    utstring_printf(s, "  %s = %s\n", #_key, utstring_body(config->_name._section##_key));         \
  }
#define x(_macro, _name, ...)                                                                      \
  if (config->type == data_config_type_##_name) {                                                  \
    DATA_##_macro##_CONFIG_INT_KEYS(save_int, _name)                                               \
        DATA_##_macro##_CONFIG_STR_KEYS(save_str, _name)                                           \
  }
    DATA_CONFIG_SCHEMA(x, )
#undef x
#undef save_str
#undef save_int
    utstring_printf(s, "\n");
  }
  return s;
}

void data_config_set_int(data_config *config, const char *section, const char *key, int value) {
#define parse_int(_section, _key, _, _name)                                                        \
  if (strcmp(#_section, section) == 0 && strcmp(#_key, key) == 0) {                                \
    config->_name._section##_key = value;                                                          \
    return;                                                                                        \
  }
#define x(_macro, _name, ...)                                                                      \
  if (config->type == data_config_type_##_name) {                                                  \
    DATA_##_macro##_CONFIG_INT_KEYS(parse_int, _name)                                              \
  }
  DATA_CONFIG_SCHEMA(x, )
#undef x
#undef parse_int
  log_warn("tried to set value '%d' for int config key '%s", value, key);
}

void data_config_set_str(data_config *config, const char *section, const char *key,
                         const char *value) {
#define parse_str(_section, _key, _, _name)                                                        \
  if (strcmp(#_section, section) == 0 && strcmp(#_key, key) == 0) {                                \
    utstring_clear(config->_name._section##_key);                                                  \
    utstring_printf(config->_name._section##_key, "%s", value);                                    \
    return;                                                                                        \
  }
#define x(_macro, _name, ...)                                                                      \
  if (config->type == data_config_type_##_name) {                                                  \
    DATA_##_macro##_CONFIG_STR_KEYS(parse_str, _name)                                              \
  }
  DATA_CONFIG_SCHEMA(x, )
#undef x
#undef parse_str
  log_error("tried to set value '%s' for string config key '%s' '%s'", value, section, key);
}

void data_config_set_default_int(data_config *config, const char *section, const char *key) {
#define parse_int(_section, _key, _default, _name)                                                 \
  if (strcmp(#_section, section) == 0 && strcmp(#_key, key) == 0) {                                \
    config->_name._section##_key = _default;                                                       \
    return;                                                                                        \
  }
#define x(_macro, _name, ...)                                                                      \
  if (config->type == data_config_type_##_name) {                                                  \
    DATA_##_macro##_CONFIG_INT_KEYS(parse_int, _name)                                              \
  }
  DATA_CONFIG_SCHEMA(x, )
#undef x
#undef parse_int
  log_warn("tried to set default value for int config key '%s", key);
}
void data_config_set_default_str(data_config *config, const char *section, const char *key) {
#define parse_str(_section, _key, _default, _name)                                                 \
  if (strcmp(#_section, section) == 0 && strcmp(#_key, key) == 0) {                                \
    utstring_clear(config->_name._section##_key);                                                  \
    utstring_printf(config->_name._section##_key, "%s", _default);                                 \
    return;                                                                                        \
  }
#define x(_macro, _name, ...)                                                                      \
  if (config->type == data_config_type_##_name) {                                                  \
    DATA_##_macro##_CONFIG_STR_KEYS(parse_str, _name)                                              \
  }
  DATA_CONFIG_SCHEMA(x, )
#undef x
#undef parse_str
  log_error("tried to set default value for string config key '%s' '%s'", section, key);
}

void data_config_set_default_values(data_config *config) {
#define set_int(_section, _key, _default, _name) config->_name._section##_key = _default;
#define set_str(_section, _key, _default, _name)                                                   \
  utstring_clear(config->_name._section##_key);                                                    \
  utstring_printf(config->_name._section##_key, "%s", _default);
#define x(_macro, _name, ...)                                                                      \
  if (config->type == data_config_type_##_name) {                                                  \
    DATA_##_macro##_CONFIG_INT_KEYS(set_int, _name)                                                \
        DATA_##_macro##_CONFIG_STR_KEYS(set_str, _name)                                            \
  }
  DATA_CONFIG_SCHEMA(x, )
#undef x
#undef set_int
#undef set_str
  config->dirty = true;
}
