#include "../peg/c_parser.h"
#include "../platform.h"

struct Foo {
  int a;
};
int main(int argc, char *argv[]) {
  printf("src path: %s\n", SRC_PATH);
  // TODO: Scan enums in headers and generate strings.
  size_t inputSize;
  platform_path input_path = get_executable_dir_path();
  platform_path_append(&input_path, "/tests/c_parser_test.txt");
  char *input = read_text_file(&input_path, &inputSize);
  platform_path_free(&input_path);
  if (input == NULL) {
    fprintf(stderr, "failed to load file");
    exit(EXIT_FAILURE);
  }
  c_parser_state state = c_parser_execute(input);
  c_parser_debug_print(&state);
  c_parser_state_free(&state);
  return 0;
}
