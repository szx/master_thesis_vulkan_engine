/* Basic cross-platform functions. */

#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(__linux) || defined(__linux__) || defined(linux)
#define PLATFORM_LINUX
#elif defined(__APPLE__)
#define PLATFORM_APPLE
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#endif

#ifndef SRC_PATH
#error "SRC_PATH should be defined by CMake"
#define SRC_PATH "SRC_PATH should be defined by CMake"
#endif
#ifndef VULKAN_HEADER_PATH
#error "VULKAN_HEADER_PATH should be defined by CMake"
#define VULKAN_HEADER_PATH "VULKAN_HEADER_PATH should be defined by CMake"
#endif

#if defined(DEBUG)
#define debug_msg(msg) log_trace("debug_msg: %s in %s:%s:%d", msg, __func__, __FILE__, __LINE__)
#else
void dummy_debug_msg();
#define debug_msg(msg) dummy_debug_msg()
#endif

#include "alloc.h"
#include "struct_alloc.h"

static_assert(sizeof(char) == sizeof(uint8_t), "sizeof(char) != sizeof(uint8_t)");

/// Allocates and initializes all global state used by functions below.
void platform_init();

/// Frees all global state used by functions below.
void platform_free();

/// Adds log.txt file to logger.
void log_init();

/// Closes log.txt file.
void log_free();

/// Shows message dialog and exits.
void panic(const char *format, ...);

#define verify(cond)                                                                               \
  do {                                                                                             \
    if (!(cond)) {                                                                                 \
      panic("%s:%d: %s failed", __FILE__, __LINE__, #cond);                                        \
    }                                                                                              \
  } while (0)

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* file system */
typedef struct platform_path {
  str data;
  struct platform_path *next;
} platform_path;

platform_path platform_path_init(const char *data);
void platform_path_deinit(platform_path *self);
platform_path platform_path_copy(platform_path *self);
const char *platform_path_c_str(platform_path *self);

void platform_path_append(platform_path *self, const char *dirOrFileName);
void platform_path_append_ext(platform_path *self, const char *ext);
bool platform_path_equals(platform_path *self, platform_path *other);
bool platform_path_dirname_equals(platform_path *self, platform_path *other);
bool platform_path_ext_equals(platform_path *self, const char *ext);
platform_path platform_path_get_dirname(platform_path *self);
str platform_path_get_basename(platform_path *self);

/// Returns new path to directory with current executable.
platform_path get_executable_dir_path();

/// Returns new path to file in specified directory.
platform_path get_executable_dir_file_path(const char *dirName, const char *fileName);

/// Returns new path to file in assets directory.
platform_path get_asset_file_path(const char *dirName, const char *fileName);

/// Returns new list of child paths.
platform_path *get_dir_children(platform_path *dirPath);

/// Returns null-terminated string with text file content.
char *read_text_file(platform_path *path, size_t *sourceLength);

/// Writes text file.
void write_text_file(platform_path *path, str *content);

#endif /* !PLATFORM_H */
