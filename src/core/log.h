/* Logging. */
#pragma once

typedef enum log_level {
  log_level_debug,
  log_level_info,
  log_level_warn,
  log_level_error,
  log_level_fatal,
  log_level_count
} log_level;

#if defined(DEBUG)
#define log_debug(...) log_pretty(log_level_debug, __func__, __FILE__, __LINE__, __VA_ARGS__)
#else
#define log_debug(...)                                                                             \
  do {                                                                                             \
  } while (0)
#endif

#define log_info(...) log_pretty(log_level_info, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_pretty(log_level_warn, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_pretty(log_level_error, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_pretty(log_level_fatal, __func__, __FILE__, __LINE__, __VA_ARGS__)

void log_raw_va(FILE *file, const char *format, va_list args);

void log_raw(FILE *file, const char *format, ...);

void log_pretty(log_level level, const char *func, const char *file, size_t line,
                const char *format, ...);

void log_set_file(UT_string *path);
void log_close_file();

void log_set_use_default_file(bool use);
