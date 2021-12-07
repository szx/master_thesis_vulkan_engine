#include "../peg/c_parser.h"
#include "../platform.h"

static platform_path srcPath;
static platform_path vulkanHeaderPath;
static lst_platform_path srcChildPathLst;
static platform_path codegenPath;

typedef struct c_parser_callbackData {
  c_parser_state *state;
  str *headerCode;
  str *sourceCode;
  bool isVulkanHeader;
} c_parser_callbackData;

void str_append_node(str *self, c_parser_state *state,
                     c_parser_ast_node *node) {
  size_t len = node->range.end - node->range.begin;
  char *str = state->source + node->range.begin;
  for (size_t i = 0; i < len; i++) {
    str_push_back(self, str[i]);
  }
}

static bool enumerator_callback(c_parser_ast_node *node, void *callbackData) {
  if (node->type != EnumeratorDeclaration) {
    return true;
  }
  c_parser_ast_node *identifier =
      lst_c_parser_ast_node_ptr_front(&node->childNodes)->node;
  assert(identifier->type == Identifier);
  c_parser_callbackData *data = callbackData;
  str_append(data->sourceCode, "  if (value == ");
  str_append_node(data->sourceCode, data->state, identifier);
  str_append(data->sourceCode, ") {\n");
  str_append(data->sourceCode, "    return \"");
  str_append_node(data->sourceCode, data->state, identifier);
  str_append(data->sourceCode, "\";\n");
  str_append(data->sourceCode, "  }\n");
  return false;
}

static bool enum_callback(c_parser_ast_node *node, void *callbackData) {
  c_parser_callbackData *data = callbackData;
  if (node->type == EnumerationDeclaration) {
    // c_parser_ast_node_debug_print(data->state, node, 0);
    c_parser_ast_node *identifier =
        lst_c_parser_ast_node_ptr_front(&node->childNodes)->node;
    assert(identifier);
    c_parser_ast_node *enumeratorList =
        lst_c_parser_ast_node_ptr_back(&node->childNodes)->node;
    assert(enumeratorList);
    // header code (function declarations)
    str_append(data->headerCode, "const char *");
    str_append_node(data->headerCode, data->state, identifier);
    str_append(data->headerCode, "_debug_str(");
    str_append_node(data->headerCode, data->state, identifier);
    str_append(data->headerCode, " value);\n");
    // source code (function definitions)
    str_append(data->sourceCode, "const char *");
    str_append_node(data->sourceCode, data->state, identifier);
    str_append(data->sourceCode, "_debug_str(");
    str_append_node(data->sourceCode, data->state, identifier);
    str_append(data->sourceCode, " value) {\n");
    c_parser_ast_node_visit(enumeratorList, enumerator_callback, data);
    str_append(data->sourceCode, "  return \"UNKNOWN_ENUM\";\n");
    str_append(data->sourceCode, "}\n");
  }
  return node->type == TranslationUnit || node->type == LanguageLinkage ||
         node->type == TypedefEnumDeclaration;
}

static bool struct_callback(c_parser_ast_node *node, void *callbackData) {
  c_parser_callbackData *data = callbackData;
  if (node->type == StructDeclaration) {
    // c_parser_ast_node_debug_print(data->state, node, 0);
    // TODO: Codegen structs.
  }
  return node->type == TranslationUnit || node->type == LanguageLinkage ||
         node->type == TypedefStructDeclaration;
}

static bool function_declaration_callback(c_parser_ast_node *node,
                                          void *callbackData) {
  c_parser_callbackData *data = callbackData;
  if (node->type == FunctionDeclaration) {
    if (data->isVulkanHeader) {
      // c_parser_ast_node_debug_print(data->state, node, 0);
      lst_c_parser_ast_node_ptr_it it =
          lst_c_parser_ast_node_ptr_it_each(&node->childNodes);
      c_parser_ast_node *declarationSpecifiers = it.ref->node;
      assert(declarationSpecifiers);
      lst_c_parser_ast_node_ptr_it_step(&it);
      c_parser_ast_node *identifier = it.ref->node;
      assert(identifier);
      lst_c_parser_ast_node_ptr_it_step(&it);
      c_parser_ast_node *parameterList = it.ref->node;
      assert(parameterList);
      // TODO: Generate Vulkan wrappers.
      // c_parser_ast_node_debug_print(data->state, identifier, 0);
    }
  }
  return node->type == TranslationUnit || node->type == LanguageLinkage ||
         node->type == TypedefStructDeclaration;
}

void str_append_header_comment(str *self) {
  str_append(self, "// This file is auto-generated.\n");
  str_append(
      self,
      "// It should be included at the end of corresponding header file.\n\n");
}

void str_append_source_comment(str *self) {
  str_append(self, "// This file is auto-generated.\n");
  str_append(self,
             "// It is not standalone translation unit - it should be\n"
             "// included at the beginning of corresponding source file.\n\n");
}

// Scans enums in header and generates code.
void parse_header(platform_path *headerPath) {
  log_info("header: %s\n", platform_path_c_str(headerPath));
  char *input = read_text_file(headerPath, NULL);
  if (input == NULL) {
    panic("failed to read file %s!", str_c_str(&headerPath->data));
  }
  c_parser_state state = c_parser_execute(input);
  // c_parser_debug_print(&state);
  if (state.errors.size > 0) {
    c_parser_debug_print(&state);
    panic("syntax errors in %s!", str_c_str(&headerPath->data));
  }

  str headerCode = str_init("");
  str sourceCode = str_init("");
  str_append_header_comment(&headerCode);
  str_append_source_comment(&sourceCode);

  // parse and generate code
  bool isVulkanHeader = platform_path_equals(headerPath, &vulkanHeaderPath);
  c_parser_callbackData data = {&state, &headerCode, &sourceCode,
                                isVulkanHeader};

  // parse enum declarations
  c_parser_ast_node_visit(state.programNode, enum_callback, &data);
  // parse structs
  c_parser_ast_node_visit(state.programNode, struct_callback, &data);
  // parse function declarations
  c_parser_ast_node_visit(state.programNode, function_declaration_callback,
                          &data);

  // write generated header_code to file
  // log_debug("CODE:\n%s\n", str_c_str(&header_code));
  str basename = platform_path_get_basename(headerPath);
  platform_path headerOutputPath = platform_path_copy(&codegenPath);
  platform_path sourceOutputPath = platform_path_copy(&codegenPath);
  platform_path_append(&headerOutputPath, str_c_str(&basename));
  platform_path_append(&sourceOutputPath, str_c_str(&basename));
  str_free(&basename);
  sourceOutputPath.data.value[sourceOutputPath.data.size - 1] = 'c';
  log_info("header output: %s\n", str_c_str(&headerOutputPath.data));
  log_info("source output: %s\n", str_c_str(&sourceOutputPath.data));
  write_text_file(&headerOutputPath, &headerCode);
  write_text_file(&sourceOutputPath, &sourceCode);

  platform_path_free(&headerOutputPath);
  platform_path_free(&sourceOutputPath);
  c_parser_state_free(&state);
  str_free(&headerCode);
  str_free(&sourceCode);
}

int main(int argc, char *argv[]) {
  platform_init();
  log_info("src path: %s\n", SRC_PATH);
  log_info("vulkan header path: %s\n", VULKAN_HEADER_PATH);

  srcPath = platform_path_init(SRC_PATH);
  vulkanHeaderPath = platform_path_init(VULKAN_HEADER_PATH);
  srcChildPathLst = get_dir_children(&srcPath);
  codegenPath = platform_path_copy(&srcPath);
  platform_path_append(&codegenPath, "codegen");
  lst_platform_path_push_front(&srcChildPathLst, vulkanHeaderPath);

  foreach (lst_platform_path, &srcChildPathLst, it) {
    if (!platform_path_dirname_equals(it.ref, &codegenPath)) {
      if (platform_path_ext_equals(it.ref, ".h")) {
        parse_header(it.ref);
      }
    }
    platform_path_free(it.ref);
  }
  platform_path_free(&codegenPath);
  lst_platform_path_free(&srcChildPathLst);
  platform_path_free(&srcPath);
  platform_free();
  return 0;
}
