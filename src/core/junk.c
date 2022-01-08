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