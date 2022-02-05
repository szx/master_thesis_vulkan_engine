#include "platform.h"

static FILE *logFile;

void platform_create() {
  globals_create();
  log_create();
}

void platform_destroy() {
  log_destroy();
  globals_destroy();
}

void log_create() {
#if defined(DEBUG)
  int logLevel = LOG_DEBUG;
#else
  int logLevel = LOG_INFO;
#endif
  log_set_level(logLevel);
  UT_string *path = get_executable_dir_file_path("", "log.txt");
  logFile = fopen(utstring_body(path), "wb");
  utstring_free(path);
  if (logFile) {
    log_add_fp(logFile, logLevel);
  }
}

void log_destroy() { fclose(logFile); }

void dummy_func() {}

typedef struct panic_args {
  char *msg;
} panic_args;

static void panic_on_activate_callback(GtkApplication *app, panic_args *panicArgs) {
  GtkWidget *window = gtk_application_window_new(app);
  GtkWidget *dialog =
      gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                             GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", panicArgs->msg);
  gtk_window_set_title(GTK_WINDOW(dialog), "Fatal error");
  g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
  g_signal_connect_swapped(dialog, "response", G_CALLBACK(g_application_quit), app);
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

  log_fatal(panicArgs.msg);
  GtkApplication *app = gtk_application_new("com.example.GtkApplication", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(panic_on_activate_callback), &panicArgs);
  g_application_run(G_APPLICATION(app), 0, NULL);
  g_object_unref(app);

  exit(EXIT_FAILURE);
}

bool path_ext_equal(UT_string *path, const char *ext) {
  const char *data = utstring_body(path) + utstring_len(path) - strlen(ext);
  return g_strcmp0(data, ext) == 0;
}

UT_string *get_executable_dir_path() {
#if defined(PLATFORM_LINUX)
  char *exePath = g_file_read_link("/proc/self/exe", NULL);
  char *exeDir = g_path_get_dirname(exePath);
  UT_string *path;
  utstring_new(path);
  utstring_printf(path, "%s", exeDir);
  g_free(exePath);
  g_free(exeDir);
  return path;
#else
#error "plaform.c does not support current platform"
#endif
}

UT_string *get_path_dirname(UT_string *path) {
#if defined(PLATFORM_LINUX)
  char *dirPath = g_path_get_dirname(utstring_body(path));
  UT_string *newPath;
  utstring_new(newPath);
  utstring_printf(newPath, "%s", dirPath);
  g_free(dirPath);
  return newPath;
#else
#error "plaform.c does not support current platform"
#endif
}

void append_to_path(UT_string *path, const char *name) { utstring_printf(path, "/%s", name); }

UT_string *get_executable_dir_file_path(const char *dirName, const char *fileName) {
  UT_string *path = get_executable_dir_path();
  append_to_path(path, dirName);
  append_to_path(path, fileName);
  return path;
}

UT_string *get_asset_file_path(const char *dirName, const char *fileName) { // HIRO remove
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

    utstring_printf(s, "%s", result);
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
