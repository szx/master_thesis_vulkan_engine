#include "platform.h"

// We try to use GLib for cross-platform functionality.
#include <glib.h>
#include <gtk/gtk.h>
#include <stdarg.h>

static FILE *logFile;

void platform_init() { log_init(); }

void platform_free() { log_free(); }

void log_init() {
  platform_path path = get_executable_dir_file_path("", "log.txt");
  logFile = fopen(platform_path_c_str(&path), "wb");
  platform_path_free(&path);
  if (logFile) {
    log_add_fp(logFile, LOG_TRACE); // TODO: Set different level on Release.
  }
}

void log_free() { fclose(logFile); }

typedef struct panic_args {
  char *msg;
} panic_args;

static void panic_on_activate_callback(GtkApplication *app,
                                       panic_args *panicArgs) {
  GtkWidget *window = gtk_application_window_new(app);
  GtkWidget *dialog = gtk_message_dialog_new(
      GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
      GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", panicArgs->msg);
  gtk_window_set_title(GTK_WINDOW(dialog), "Fatal error");
  g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
  g_signal_connect_swapped(dialog, "response", G_CALLBACK(g_application_quit),
                           app);
  gtk_widget_show(GTK_WIDGET(dialog));
}

void panic(const char *format, ...) {
  static const size_t max_shown_chars = 2048;
  va_list args;
  va_start(args, format);
  char msg[max_shown_chars];
  vsnprintf(msg, max_shown_chars, format, args);
  panic_args panicArgs = {.msg = msg};
  va_end(args);

  GtkApplication *app = gtk_application_new("com.example.GtkApplication",
                                            G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(panic_on_activate_callback),
                   &panicArgs);
  g_application_run(G_APPLICATION(app), 0, NULL);
  g_object_unref(app);

  exit(EXIT_FAILURE);
}

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

const char *platform_path_c_str(platform_path *self) {
  return str_c_str(&self->data);
}

void platform_path_append(platform_path *self, const char *dirOrFileName) {
  str_append(&self->data, G_DIR_SEPARATOR_S);
  str_append(&self->data, dirOrFileName);
}

bool platform_path_equals(platform_path *self, platform_path *other) {
  const char *data1 = str_c_str(&self->data);
  const char *data2 = str_c_str(&other->data);
  return g_strcmp0(data1, data2) == 0;
}

bool platform_path_dirname_equals(platform_path *self, platform_path *other) {
  platform_path path1 = platform_path_get_dirname(self);
  bool result = platform_path_equals(&path1, other);
  platform_path_free(&path1);
  return result;
}

bool platform_path_ext_equals(platform_path *self, const char *ext) {
  const char *data = str_c_str(&self->data) + self->data.size - strlen(ext);
  return g_strcmp0(data, ext) == 0;
}

platform_path platform_path_get_dirname(platform_path *self) {
  char *path = g_path_get_dirname(str_c_str(&self->data));
  platform_path newPath = platform_path_init(path);
  free(path);
  return newPath;
}

str platform_path_get_basename(platform_path *self) {
  char *basename = g_path_get_basename(str_c_str(&self->data));
  str name = str_init(basename);
  free(basename);
  return name;
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

platform_path get_executable_dir_file_path(const char *dirName,
                                           const char *fileName) {
  platform_path result = get_executable_dir_path();
  platform_path_append(&result, dirName);
  platform_path_append(&result, fileName);
  return result;
}

platform_path get_asset_file_path(const char *fileName) {
  return get_executable_dir_file_path("assets", fileName);
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

lst_platform_path get_dir_children(platform_path *dirPath) {
#if defined(PLATFORM_LINUX)
  lst_platform_path paths = lst_platform_path_init();
  get_dir_children_impl(str_c_str(&dirPath->data), &paths);
  return paths;
#else
#error "plaform.c does not support current platform"
#endif
}

char *read_text_file(platform_path *path, size_t *sourceLength) {
  char *result = 0;
  FILE *file = fopen(str_c_str(&path->data), "rb");

  if (file) {
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    result = (char *)malloc(size + 1);
    fread(result, size, 1, file);
    result[size] = 0;
    if (sourceLength != NULL) {
      *sourceLength = size;
    }

    fclose(file);
  }

  return result;
}

void write_text_file(platform_path *path, str *content) {
  FILE *file = fopen(str_c_str(&path->data), "wb");
  if (file) {
    fprintf(file, "%s", str_c_str(content));
    fclose(file);
  }
}
