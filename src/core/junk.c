#include "junk.h"

void lstrip(char **str) {
  char *val = *str;
  while (val[0] == ' ' || val[0] == '\t') {
    val++;
  }
  *str = val;
}

void rstrip(char **str) {
  char *val = *str;
  for (size_t i = strlen(val) - 1; i != 0; i--) {
    if (val[i] == ' ' || val[0] == '\t') {
      val[i] = '\0';
    }
  }
  *str = val;
}
