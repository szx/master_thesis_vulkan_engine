#include "platform.h"

// note(sszczyrb): We try to use GLib for cross-platform functionality.
#include <glib.h>

platform_path platform_path_init(char *data) {
  platform_path path = {0};
  path.data = str_init(data);
  return path;
}

void platform_path_free(platform_path *self) { str_free(&self->data); }

platform_path platform_path_copy(platform_path *self) {
  platform_path copy = {0};
  copy.data = str_copy(&self->data);
  return copy;
}

void platform_path_append(platform_path *self, const char *dir_or_file_name) {
  str_append(&self->data, dir_or_file_name);
}

platform_path get_executable_dir_path() {
#if defined(PLATFORM_LINUX)
  char *exePath = g_file_read_link("/proc/self/exe", NULL);
  char *exeDir = g_path_get_dirname(exePath);
  platform_path path = platform_path_init(exeDir);
  g_free(exePath);
  g_free(exeDir);
  return path;
#else
#error "plaform.c does not support current platform"
#endif
}

vec_platform_path get_dir_children(platform_path *dir_path) {
#if defined(PLATFORM_LINUX)
  vec_platform_path paths = vec_platform_path_init();
  platform_path childPath = platform_path_init("todo");
  vec_platform_path_push_back(&paths, childPath);
  // TODO: implement
  return paths;
#else
#error "plaform.c does not support current platform"
#endif
}

char *read_text_file(platform_path *path, size_t *source_length) {
  char *result = 0;
  FILE *file = fopen(str_c_str(&path->data), "rb");

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
