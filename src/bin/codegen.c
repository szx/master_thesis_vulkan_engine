#include "../peg/c_parser.h"
#include "../platform.h"

// Scans enums in header and generate strings.
void parse_header(platform_path *headerPath) {
  printf("header: %s\n", str_c_str(&headerPath->data));
  char *input = read_text_file(headerPath, NULL);
  if (input == NULL) {
    fprintf(stderr, "failed to load file");
    exit(EXIT_FAILURE);
  }
  c_parser_state state = c_parser_execute(input);
  // TODO: Generate enum to string functions.
  c_parser_debug_print(&state);
  c_parser_state_free(&state);
}

int main(int argc, char *argv[]) {
  printf("src path: %s\n", SRC_PATH);
  platform_path srcPath = platform_path_init(SRC_PATH);
  lst_platform_path srcChildPathLst = get_dir_children(&srcPath);
  foreach (lst_platform_path, &srcChildPathLst, it) {
    if (platform_path_ext_equals(it.ref, ".h")) {
      parse_header(it.ref);
    }
    platform_path_free(it.ref);
  }
  lst_platform_path_free(&srcChildPathLst);
  platform_path_free(&srcPath);
  return 0;
}
