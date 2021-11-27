#include "peg/c_parser.h"
#include "platform.h"

struct Foo {
  int a;
};
int main(int argc, char *argv[]) {
  printf("src path: %s\n", SRC_PATH);
  // TODO: Scan enums in headers and generate strings.
  size_t inputSize;
  str inputPath = str_init(get_executable_dir_path());
  str_append(&inputPath, "/tests/c_parser_test.txt");
  // str_append(&input_path, "/tests/c_parser_test_functions.txt");
  char *input = read_text_file(str_c_str(&inputPath), &inputSize);
  str_free(&inputPath);
  if (input == NULL) {
    fprintf(stderr, "failed to load file");
    exit(EXIT_FAILURE);
  }
  c_parser_state state = c_parser_execute(input);
  // c_parser_debug_print(&state);
  return 0;
}
