#include "peg/c_parser.h"
#include "platform.h"

int main(int argc, char *argv[]) {
  // TODO: Scan enums in headers and generate strings.
  size_t input_size;
  str input_path = str_init(getExecutableDirPath());
  str_append(&input_path, "/tests/c_parser_test.txt");
  // str_append(&input_path, "/tests/c_parser_test_functions.txt");
  char *input = ReadTextFile(str_c_str(&input_path), &input_size);
  str_free(&input_path);
  if (input == NULL) {
    fprintf(stderr, "failed to load file");
    exit(EXIT_FAILURE);
  }
  c_parser_state state = c_parser_execute(input);
  c_parser_debug_print(&state);
  return 0;
}
