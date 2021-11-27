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

#include "str.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* file system */
typedef struct platform_path {
  str data;
} platform_path;

platform_path platform_path_init(const char *data);

void platform_path_free(platform_path *self);

platform_path platform_path_copy(platform_path *self);

void platform_path_append(platform_path *self, const char *dir_or_file_name);
bool platform_path_ext_equals(platform_path *self, const char *ext);

#define T platform_path
#include "lst.h" // lst_platform_path

// Returns new path to directory with current executable.
platform_path get_executable_dir_path();

// Returns new list of child paths.
lst_platform_path get_dir_children(platform_path *dir_path);

// Returns null-terminated string with text file content.
char *read_text_file(platform_path *path, size_t *source_length);

#endif /* !PLATFORM_H */
