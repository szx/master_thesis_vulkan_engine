#include "../peg/c_parser.h"
#include "../platform.h"
#include "greatest.h"
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
  image copy = {0};
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
  vec_int_push_back(&a, 9); // NOLINT
  vec_int_push_back(&a, 1); // NOLINT
  vec_int_push_back(&a, 8); // NOLINT
  vec_int_push_back(&a, 3); // NOLINT
  vec_int_push_back(&a, 4); // NOLINT
  vec_int_sort(&a, vec_int_compare);
  int last = 0;
  for (size_t i = 0; i < a.size; i++) {
    ASSERT_LT(last, a.value[i]);
    last = a.value[i];
  }
  vec_int_free(&a);

  vec_image b = vec_image_init();
  vec_image_push_back(&b, image_init(10));  // NOLINT
  vec_image_push_back(&b, image_init(10));  // NOLINT
  vec_image_push_back(&b, image_init(100)); // NOLINT
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

// Parsing C preprocessor directives.
TEST c_parser_preprocessor_parsing() {
  size_t inputSize;
  platform_path inputPath = get_executable_dir_path();
  platform_path_append(&inputPath, "/tests/c_parser_test.txt");
  char *input = read_text_file(&inputPath, &inputSize);
  platform_path_free(&inputPath);
  if (input == NULL) {
    FAILm("failed to load file");
  }
  c_parser_state state = c_parser_execute(input);
  c_parser_debug_print(&state);
  c_parser_state_free(&state);
  PASS();
}

SUITE(c_parser_suite) { RUN_TEST(c_parser_preprocessor_parsing); }

GREATEST_MAIN_DEFS(); // NOLINT

int main(int argc, char *argv[]) {
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(basic_test_suite);
  RUN_SUITE(c_parser_suite);

  GREATEST_MAIN_END();
}
