#include "junk.h"

void strlstrip(char **str) {
  char *val = *str;
  while (val[0] == ' ' || val[0] == '\t') {
    val++;
  }
  *str = val;
}

void strrstrip(char **str) {
  char *val = *str;
  size_t len = strlen(val);
  if (len == 0) {
    return;
  }
  for (size_t i = strlen(val) - 1; i != 0; i--) {
    if (val[i] == ' ' || val[0] == '\t') {
      val[i] = '\0';
    } else {
      break;
    }
  }
  *str = val;
}

void strstrip(char **str) {
  strlstrip(str);
  strrstrip(str);
}

size_t count_bits(size_t bits) {
  // http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
  size_t count;
  for (count = 0; bits; count++) {
    bits &= bits - 1;
  }
  return count;
}

size_t _dl_count(size_t offsetofNext, void *dl) {
  void *it = dl;
  size_t count = 0;
  while (it != NULL) {
    count++;
    char *ptr = (char *)it + offsetofNext;
    it = *(void **)(ptr);
  }
  return count;
}

void *_dl_elt(size_t offsetofNext, void *dl, size_t i) {
  void *it = dl;
  size_t count = 0;
  while (it != NULL) {
    if (count == i) {
      return it;
    }
    count++;
    char *ptr = (char *)it + offsetofNext;
    it = *(void **)(ptr);
  }
  assert(0);
  return NULL;
}
