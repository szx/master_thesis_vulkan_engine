#include "platform.h"

#if defined(PLATFORM_LINUX)

#include <libgen.h>
#include <linux/limits.h>
#include <unistd.h>

char *getExecutableDirPath() {
  static char result[PATH_MAX] = {0};
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  assert(count != -1); // TODO: Custom assert + dialog?
  const char *path = dirname(result);
  return path;
}

#else
#error "plaform.c does not support current platform"
#endif
