#include "platform.h"

static UT_string *executableDirPath;
#if defined(PLATFORM_LINUX)
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
#elif defined(PLATFORM_WINDOWS)
#include <windows.h>
#else
#error "plaform.c does not support current platform"
#endif

void set_executable_dir_path(int argc, char *argv[]) {
#if defined(PLATFORM_LINUX)
  char exePath[PATH_MAX];
  ssize_t exePathLen = readlink("/proc/self/exe", exePath, sizeof(exePath));
  verify(exePathLen != -1);
  char *exeDir = dirname(exePath);

  UT_string *path;
  utstring_new(path);
  utstring_printf(path, "%s", exeDir);
  executableDirPath = path;
  return;
#elif defined(PLATFORM_WINDOWS)
  char exePath[PATH_MAX];
  DWORD result = GetModuleFileNameA(NULL, exePath, sizeof(exePath));
  verify(result != 0);
  PathRemoveFileSpecA(exePath);

  UT_string *path;
  utstring_new(path);
  utstring_printf(path, "%s", exeDir);
  executableDirPath = path;
  return;
#endif
  UNREACHABLE;
}

void platform_create(int argc, char *argv[]) {
  set_executable_dir_path(argc, argv);
  globals_create();
  log_create();
}

void platform_destroy() {
  log_destroy();
  globals_destroy();
}

void log_create() {
  UT_string *path = get_executable_dir_file_path("", "log.txt");
  log_set_file(path);
  utstring_free(path);
}

void log_destroy() { log_close_file(); }

typedef struct panic_args {
  char *msg;
} panic_args;

noreturn void panic(const char *format, ...) {
  static const size_t max_shown_chars = 2048;
  va_list args;
  va_start(args, format);
  char msg[max_shown_chars];
  vsnprintf(msg, max_shown_chars, format, args);
  panic_args panicArgs = {.msg = msg};
  va_end(args);

  log_fatal(panicArgs.msg);
  log_destroy();

  exit(EXIT_FAILURE);
}

bool path_ext_equal(UT_string *path, const char *ext) {
  const char *data = utstring_body(path) + utstring_len(path) - strlen(ext);
  return strcmp(data, ext) == 0;
}

UT_string *get_executable_dir_path() {
  UT_string *path;
  utstring_alloc(path, utstring_body(executableDirPath));
  return path;
}

UT_string *get_path_dirname(UT_string *path) {
#if defined(PLATFORM_LINUX)
  char *pathDup = core_strdup(utstring_body(path));
  char *dir = dirname(pathDup);

  UT_string *newPath;
  utstring_new(newPath);
  utstring_printf(newPath, "%s", dir);
  core_free(pathDup);
  return newPath;
#elif defined(PLATFORM_WINDOWS)
  char dir[MAX_PATH];
  PathRemoveFileSpecA(lstrcpyA(dir, utstring_body(path)));

  UT_string *newPath;
  utstring_new(newPath);
  utstring_printf(newPath, "%s", dir);
  core_free(pathDup);
  return newPath;
#endif
  UNREACHABLE;
}

void append_to_path(UT_string *path, const char *name) { utstring_printf(path, "/%s", name); }

UT_string *get_executable_dir_file_path(const char *dirName, const char *fileName) {
  UT_string *path = get_executable_dir_path();
  append_to_path(path, dirName);
  append_to_path(path, fileName);
  return path;
}

UT_string *get_asset_file_path(const char *dirName, const char *fileName) {
  UT_string *path = get_executable_dir_file_path("assets", dirName);
  append_to_path(path, fileName);
  return path;
}

UT_string *read_text_file(UT_string *path) {
  UT_string *s;
  utstring_new(s);

  char *result = 0;
  FILE *file = fopen(utstring_body(path), "rb");
  if (file) {
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    result = (char *)malloc(size + 1);
    if (fread(result, size, 1, file) != 1) {
      return NULL;
    }
    result[size] = 0;
    fclose(file);

    utstring_bincpy(s, result, size + 1);
    free(result);
  }

  return s;
}

bool write_text_file(UT_string *path, UT_string *content) {
  FILE *file = fopen(utstring_body(path), "wb");
  if (file) {
    fprintf(file, "%s", utstring_body(content));
    fclose(file);
    return true;
  }
  return false;
}
