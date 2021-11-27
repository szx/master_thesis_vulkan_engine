#include "platform.h"

// note(sszczyrb): We try to use GLib for cross-platform functionality.
#include <glib.h>

platform_path platform_path_init(const char *data) {
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
  str_append(&self->data, G_DIR_SEPARATOR_S);
  str_append(&self->data, dir_or_file_name);
}

bool platform_path_ext_equals(platform_path *self, const char *ext) {
  const char *data = str_c_str(&self->data) + self->data.size - strlen(ext);
  return g_strcmp0(data, ext) == 0;
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

void get_dir_children_impl(const char *path, lst_platform_path *paths) {
  GError *error = NULL;
  GDir *dir = g_dir_open(path, 0, &error);
  if (error != NULL) {
    // file
    platform_path childPath = platform_path_init(path);
    lst_platform_path_push_back(paths, childPath);
  } else {
    const char *childName;
    while ((childName = g_dir_read_name(dir))) {
      gchar *childPath = g_strconcat(path, G_DIR_SEPARATOR_S, childName, NULL);
      get_dir_children_impl(childPath, paths);
      g_free(childPath);
    }
  }
}

lst_platform_path get_dir_children(platform_path *dir_path) {
#if defined(PLATFORM_LINUX)
  lst_platform_path paths = lst_platform_path_init();
  get_dir_children_impl(str_c_str(&dir_path->data), &paths);
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
    if (source_length != NULL) {
      *source_length = size;
    }

    fclose(file);
  }

  return result;
}
