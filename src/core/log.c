#include "log.h"
#include "platform.h"

static FILE *outputFile;
static bool useDefaultFile;

void log_raw_va(FILE *file, const char *format, va_list args) {
  va_list copy;
  va_copy(copy, args);
  if (useDefaultFile) {
    vfprintf(file, format, copy);
  }
  va_end(copy);

  va_copy(copy, args);
  vfprintf(outputFile, format, copy);
  va_end(copy);
}

void log_raw(FILE *file, const char *format, ...) {
  va_list args;
  va_start(args, format);

  if (useDefaultFile) {
    log_raw_va(file, format, args);
  }
  log_raw_va(outputFile, format, args);

  va_end(args);
}

void log_pretty(log_level level, const char *func, const char *file, size_t line,
                const char *format, ...) {
  va_list args;
  va_start(args, format);

  static const char *levelStr[log_level_count] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
  FILE *defaultFile = stdout;
  if (level >= log_level_error) {
    defaultFile = stderr;
  }

  static struct {
    log_level level;
    const char *func;
    const char *file;
    size_t line;
  } last = {0};

  if (level != last.level || func != last.func || file != last.file) {
    log_raw(defaultFile, "[%s] (%s:%zu) %s:\n", levelStr[level], file, line, func);
    last.level = level;
    last.func = func;
    last.file = file;
    last.line = line;
  }
  log_raw_va(defaultFile, format, args);
  log_raw(defaultFile, "\n");

  va_end(args);
}

void log_set_file(UT_string *path) {
  outputFile = fopen(utstring_body(path), "wb");
  verify(outputFile);
}

void log_set_use_default_file(bool use) { useDefaultFile = use; }

void log_close_file() { fclose(outputFile); }
