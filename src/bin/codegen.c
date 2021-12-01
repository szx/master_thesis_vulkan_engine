#include "../peg/c_parser.h"
#include "../platform.h"

typedef struct enum_callback_data {
  c_parser_state *state;
  str *code;
} enum_callback_data;

void str_append_node(str *self, c_parser_state *state,
                     c_parser_ast_node *node) {
  size_t len = node->range.end - node->range.begin;
  char *str = state->source + node->range.begin;
  for (size_t i = 0; i < len; i++) {
    str_push_back(self, str[i]);
  }
}

static bool enumerator_callback(c_parser_ast_node *node, void *callback_data) {
  if (node->type != Identifier) {
    return true;
  }
  enum_callback_data *data = callback_data;
  str_append(data->code, "  case ");
  str_append_node(data->code, data->state, node);
  str_append(data->code, ":\n");
  str_append(data->code, "    return \"");
  str_append_node(data->code, data->state, node);
  str_append(data->code, "\";\n");
  return false;
}

static bool enum_callback(c_parser_ast_node *node, void *callback_data) {
  enum_callback_data *data = callback_data;
  if (node->type == EnumerationDeclaration) {
    // c_parser_ast_node_debug_print(data->state, node, 0);
    c_parser_ast_node *identifier =
        lst_c_parser_ast_node_ptr_front(&node->childNodes)->node;
    assert(identifier);
    str_append(data->code, "const char *");
    str_append_node(data->code, data->state, identifier);
    str_append(data->code, "_debug_str(");
    str_append_node(data->code, data->state, identifier);
    str_append(data->code, " value) {\n");
    str_append(data->code, "  switch (value) {\n");
    c_parser_ast_node *enumeratorList =
        lst_c_parser_ast_node_ptr_back(&node->childNodes)->node;
    assert(enumeratorList);
    c_parser_ast_node_visit(enumeratorList, enumerator_callback, data);
    str_append(data->code, "  default:\n");
    str_append(data->code, "    return \"UNKNOWN\";\n");
    str_append(data->code, "  }\n");
    str_append(data->code, "}\n");
  }
  return node->type == TranslationUnit || node->type == LanguageLinkage ||
         node->type == TypedefEnumDeclaration;
}

// Scans enums in header and generates code.
void parse_header(platform_path *headerPath, platform_path *codegenPath) {
  printf("header: %s\n", str_c_str(&headerPath->data));
  char *input = read_text_file(headerPath, NULL);
  if (input == NULL) {
    panic("failed to read file %s!", str_c_str(&headerPath->data));
  }
  c_parser_state state = c_parser_execute(input);
  // c_parser_debug_print(&state);
  if (state.errors.size > 0) {
    panic("syntax errors in %s!", str_c_str(&headerPath->data));
  }

  str code = str_init("");
  // parse enum declarations
  enum_callback_data data = {&state, &code};
  c_parser_ast_node_visit(state.programNode, enum_callback, &data);
  // write generated code to file

  // printf("CODE:\n%s\n", str_c_str(&code));
  str basename = platform_path_get_basename(headerPath);
  platform_path outputPath = platform_path_copy(codegenPath);
  platform_path_append(&outputPath, str_c_str(&basename));
  str_free(&basename);
  printf("output: %s\n", str_c_str(&outputPath.data));
  write_text_file(&outputPath, &code);
  // TODO: Split function declarations and definitions?
  // TODO: Run codegen via CMake.
  platform_path_free(&outputPath);
  c_parser_state_free(&state);
  str_free(&code);
}

int main(int argc, char *argv[]) {
  printf("src path: %s\n", SRC_PATH);
  printf("vulkan header path: %s\n", VULKAN_HEADER_PATH);

  platform_path srcPath = platform_path_init(SRC_PATH);
  lst_platform_path srcChildPathLst = get_dir_children(&srcPath);
  platform_path vulkanHeaderPath = platform_path_init(VULKAN_HEADER_PATH);
  platform_path codegenPath = platform_path_copy(&srcPath);
  platform_path_append(&codegenPath, "codegen");
  lst_platform_path_push_front(&srcChildPathLst, vulkanHeaderPath);
  foreach (lst_platform_path, &srcChildPathLst, it) {
    if (!platform_path_dirname_equals(it.ref, &codegenPath)) {
      if (platform_path_ext_equals(it.ref, ".h")) {
        parse_header(it.ref, &codegenPath);
      }
    }
    platform_path_free(it.ref);
  }
  platform_path_free(&codegenPath);
  lst_platform_path_free(&srcChildPathLst);
  platform_path_free(&srcPath);
  return 0;
}
