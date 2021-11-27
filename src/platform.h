// note(sszczyrb): This file should be included only in source files.

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

// Returns null-terminated path to directory with current executable.
const char *get_executable_dir_path();

// Returns list of child paths.
const char *get_dir_children(const char *dir_path);

// Returns null-terminated string with text file content.
char *read_text_file(char *path, size_t *source_length);