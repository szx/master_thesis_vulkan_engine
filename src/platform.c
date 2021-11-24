#include "platform.h"

#if defined(PLATFORM_LINUX)

#include <libgen.h>
#include <linux/limits.h>
#include <unistd.h>

const char *getExecutableDirPath() {
  static char result[PATH_MAX] = {0};
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  assert(count != -1); // TODO: Custom assert + dialog?
  const char *path = dirname(result);
  return path;
}

char *ReadTextFile(char *path, size_t *source_length) {
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

#else
#error "plaform.c does not support current platform"
#endif
