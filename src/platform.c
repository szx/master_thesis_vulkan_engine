#include "platform.h"

// note(sszczyrb): We try to use GLib for cross-platform functionality.
#include <glib.h>

const char *get_executable_dir_path() {
#if defined(PLATFORM_LINUX)
  static char *exeDir = NULL;
  char *exePath = g_file_read_link("/proc/self/exe", NULL);
  exeDir = g_path_get_dirname(exePath);
  g_free(exePath);
  return exeDir;
#else
#error "plaform.c does not support current platform"
#endif
}

const char *get_dir_children(const char *dir_path) {
#if defined(PLATFORM_LINUX)
  static char *exeDir = NULL;
  // TODO: implement
  return exeDir;
#else
#error "plaform.c does not support current platform"
#endif
}

char *read_text_file(char *path, size_t *source_length) {
  char *result = 0;
  FILE *file = fopen(path, "rb");

  if (file) {
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    result = (char *)malloc(size + 1);
    fread(result, size, 1, file);
    result[size] = 0;
    *source_length = size;

    fclose(file);
  }

  return result;
}
