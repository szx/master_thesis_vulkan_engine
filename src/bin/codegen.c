#include "../peg/c_parser.h"
#include "../platform.h"

typedef struct enum_callback_data {
  c_parser_state *state;
} enum_callback_data;

static bool enum_callback(c_parser_ast_node *node, void *callback_data) {
  enum_callback_data *data = callback_data;
  if (node->type == EnumerationDeclaration) {
    // TODO: Write to file.
    c_parser_ast_node_debug_print(data->state, node, 0);
  }
  return node->type == TranslationUnit || node->type == LanguageLinkage ||
         node->type == TypedefEnumDeclaration;
}

// Scans enums in header and generate strings.
void parse_header(platform_path *headerPath) {
  printf("header: %s\n", str_c_str(&headerPath->data));
  char *input = read_text_file(headerPath, NULL);
  if (input == NULL) {
    panic("failed to read file %s!", str_c_str(&headerPath->data));
  }
  c_parser_state state = c_parser_execute(input);
  // TODO: Generate enum to string functions.
  // c_parser_debug_print(&state);
  if (state.errors.size > 0) {
    panic("syntax errors in %s!", str_c_str(&headerPath->data));
  }
  enum_callback_data data = {&state};
  c_parser_ast_node_visit(state.programNode, enum_callback, &data);
  c_parser_state_free(&state);
}

int main(int argc, char *argv[]) {
  printf("src path: %s\n", SRC_PATH);
  printf("vulkan header path: %s\n", VULKAN_HEADER_PATH);

  platform_path srcPath = platform_path_init(SRC_PATH);
  lst_platform_path srcChildPathLst = get_dir_children(&srcPath);
  platform_path vulkanHeaderPath = platform_path_init(VULKAN_HEADER_PATH);
  lst_platform_path_push_front(&srcChildPathLst, vulkanHeaderPath);
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
