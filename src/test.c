#include "greatest.h"
#include "peg/c_parser.h"
#include "platform.h"
#include "vulkan/vulkan.h"

#define P
#define T int
#include "vec.h"
int vec_int_compare(int *a, int *b) { return *b < *a; }

typedef struct image {
  char *data;
  size_t dataSize;
} image;

void image_free(image *self) {
  free(self->data);
  self->data = NULL;
  self->dataSize = 0;
}

image image_copy(image *self) {
  image copy = {};
  copy.data = malloc(self->dataSize);
  memcpy(copy.data, self->data, self->dataSize);
  copy.dataSize = self->dataSize;
  return copy;
}

#define T image
#include "vec.h"

image image_init(size_t data_count) {
  image self = {.data = malloc(sizeof(*self.data) * data_count),
                .dataSize = sizeof(*self.data) * data_count};
  return self;
}

// Basic test template.
TEST basic_test_template() { // NOLINT

  vec_int a = vec_int_init();
  vec_int_push_back(&a, 9);
  vec_int_push_back(&a, 1);
  vec_int_push_back(&a, 8);
  vec_int_push_back(&a, 3);
  vec_int_push_back(&a, 4);
  vec_int_sort(&a, vec_int_compare);
  for (size_t i = 0, last = 0; i < a.size; i++) {
    ASSERT_LT(last, a.value[i]);
    last = a.value[i];
  }
  vec_int_free(&a);

  vec_image b = vec_image_init();
  vec_image_push_back(&b, image_init(10));
  vec_image_push_back(&b, image_init(10));
  vec_image_push_back(&b, image_init(100));
  vec_image b2 = vec_image_copy(&b);
  for (size_t i = 0; i < b.size; i++) {
    ASSERT_EQ(b.value[i].dataSize, b2.value[i].dataSize);
    ASSERT_MEM_EQ(b.value[i].data, b2.value[i].data,
                  sizeof(*b.value[i].data) * b.value[i].dataSize);
  }
  PASS();
}

SUITE(basic_test_suite) { RUN_TEST(basic_test_template); }

#include <stdlib.h>
// Returns null-terminated string.
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

// Parsing C preprocessor directives.
TEST c_parser_preprocessor_parsing() { // NOLINT
  size_t input_size;
  str input_path = str_init(getExecutableDirPath());
  str_append(&input_path, "/tests/c_parser_test.txt");
  // str_append(&input_path, "/tests/c_parser_test_functions.txt");
  char *input = ReadTextFile(str_c_str(&input_path), &input_size);
  str_free(&input_path);
  if (input == NULL) {
    FAILm("failed to load file");
  }
  c_parser_state state = c_parser_execute(input);
  c_parser_debug_print(&state);
  PASS();
}

SUITE(c_parser_suite) { RUN_TEST(c_parser_preprocessor_parsing); }

GREATEST_MAIN_DEFS(); // NOLINT

int main(int argc, char *argv[]) {
  GREATEST_MAIN_BEGIN();

#if defined(PLATFORM_LINUX)
  printf("Platform: LINUX\n");
#elif defined(PLATFORM_APPLE)
  printf("Platform: APPLE\n");
#elif defined(PLATFORM_WINDOWS)
  printf("Platform: WINDOWS\n");
#endif

  RUN_SUITE(basic_test_suite);
  RUN_SUITE(c_parser_suite);

  GREATEST_MAIN_END();
}
