#include "../core/platform.h"
#include "../peg/c_parser.h"

// Toggle after updating vulkan headers.
static const bool ignore_vulkan_header = true;

static platform_path srcPath;
static platform_path vulkanHeaderPath;
static platform_path *srcChildPathLst;
static platform_path codegenPath;

typedef struct c_parser_callbackData {
  parser_state *state;
  str *headerCode;
  str *sourceCode;
  str *structDefCode;
  bool isVulkanHeader;
} c_parser_callbackData;

void str_append_node(str *self, parser_state *state, parser_ast_node *node) {
  size_t len = node->range.end - node->range.begin;
  char *str = state->source + node->range.begin;
  for (size_t i = 0; i < len; i++) {
    str_push_back(self, str[i]);
  }
}

bool node_starts_with(parser_state *state, parser_ast_node *node, const char *prefix) {
  size_t len = node->range.end - node->range.begin;
  char *str = state->source + node->range.begin;
  if (len < strlen(prefix)) {
    return false;
  }
  for (size_t i = 0; i < strlen(prefix); i++) {
    if (str[i] != prefix[i]) {
      return false;
    }
  }
  return true;
}

static bool enumerator_callback(parser_ast_node *node, void *callbackData) {
  if (node->type != EnumeratorDeclaration) {
    return true;
  }
  parser_ast_node *identifier = node->childNodes;
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

static bool enumerator_callback2(parser_ast_node *node, void *callbackData) {
  if (node->type != EnumeratorDeclaration) {
    return true;
  }
  parser_ast_node *identifier = node->childNodes;
  assert(identifier->type == Identifier);
  c_parser_callbackData *data = callbackData;

  str_append(data->structDefCode, "ENUMERATOR(");
  str_append_node(data->structDefCode, data->state, identifier);
  str_append(data->structDefCode, ")\\\n");
  return false;
}

static bool enum_callback(parser_ast_node *node, void *callbackData) {
  c_parser_callbackData *data = callbackData;
  if (node->type == EnumerationDeclaration) {
    // parser_ast_node_debug_print(data->state, node, 0);
    parser_ast_node *identifier = node->childNodes;
    assert(identifier);
    if (node_starts_with(data->state, identifier, "core_")) {
      goto end;
    }
    parser_ast_node *enumeratorList = identifier->next;
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
    parser_ast_node_visit(enumeratorList, enumerator_callback, data);
    str_append(data->sourceCode, "  return \"UNKNOWN_ENUM\";\n");
    str_append(data->sourceCode, "}\n");

    str_append(data->structDefCode, "#define ENUM_");
    str_append_node(data->structDefCode, data->state, identifier);
    str_append(data->structDefCode, "()\\\n");
    parser_ast_node_visit(enumeratorList, enumerator_callback2, data);
    str_append(data->structDefCode, "\n");
  }
end:
  return node->type == TranslationUnit || node->type == LanguageLinkage ||
         node->type == TypedefEnumDeclaration;
}

static bool struct_callback(parser_ast_node *node, void *callbackData) {
  c_parser_callbackData *data = callbackData;
  if (node->type == StructDeclaration) {
    if (!data->isVulkanHeader) {
      parser_ast_node *identifier = node->childNodes;
      assert(identifier);
      if (node_starts_with(data->state, identifier, "core_")) {
        goto end;
      }
      // parser_ast_node_debug_print(data->state, identifier, 0);
      str_append(data->structDefCode, "#define STRUCT_");
      str_append_node(data->structDefCode, data->state, identifier);
      str_append(data->structDefCode, " 1\n");
      str_append(data->structDefCode, "#ifdef STRUCT_COND\n");
      str_append(data->structDefCode, "#  if STRUCT_COND(");
      str_append_node(data->structDefCode, data->state, identifier);
      str_append(data->structDefCode, ") == 1\n");
      str_append(data->structDefCode, "   STRUCT_COND_TRUE(");
      str_append_node(data->structDefCode, data->state, identifier);
      str_append(data->structDefCode, ")\n");
      str_append(data->structDefCode, "#  else\n");
      str_append(data->structDefCode, "   STRUCT_COND_FALSE(");
      str_append_node(data->structDefCode, data->state, identifier);
      str_append(data->structDefCode, ")\n");
      str_append(data->structDefCode, "#  endif\n");
      str_append(data->structDefCode, "#endif\n");
      str_append(data->structDefCode, "STRUCT(");
      str_append_node(data->structDefCode, data->state, identifier);
      str_append(data->structDefCode, ")\n\n");
    }
  }
end:
  return node->type == TranslationUnit || node->type == LanguageLinkage ||
         node->type == TypedefStructDeclaration;
}

static bool function_declaration_callback(parser_ast_node *node, void *callbackData) {
  c_parser_callbackData *data = callbackData;
  if (node->type == FunctionDeclaration) {
    if (!data->isVulkanHeader) {
      // parser_ast_node_debug_print(data->state, node, 0);
      parser_ast_node *declarationSpecifiers = node->childNodes;
      assert(declarationSpecifiers);
      parser_ast_node *identifier = declarationSpecifiers->next;
      assert(identifier);
      parser_ast_node *parameterList = identifier->next;
      assert(parameterList);
      // parser_ast_node_debug_print(data->state, identifier, 0);
      str_append(data->structDefCode, "#define FUNC_");
      str_append_node(data->structDefCode, data->state, identifier);
      str_append(data->structDefCode, " 1\n");
      str_append(data->structDefCode, "FUNC(");
      str_append_node(data->structDefCode, data->state, identifier);
      str_append(data->structDefCode, ")\n\n");
    }
    // TODO: Generate Vulkan wrappers.
  }
  return node->type == TranslationUnit || node->type == LanguageLinkage ||
         node->type == TypedefStructDeclaration;
}

void str_append_header_comment(str *self) {
  str_append(self, "// This file is auto-generated.\n");
  str_append(self, "// It should be included at the end of corresponding header file.\n\n");
}

void str_append_source_comment(str *self) {
  str_append(self, "// This file is auto-generated.\n");
  str_append(self, "// It is not standalone translation unit - it should be\n"
                   "// included at the beginning of corresponding source file.\n\n");
}

void str_append_meta_defines(str *self) {
  str_append(self, "// This file is auto-generated.\n");
  str_append(self, "// It contains X-macros.\n");
  str_append(self, "#ifndef STRUCT\n");
  str_append(self, "#define STRUCT(name)\n");
  str_append(self, "#endif\n");
  str_append(self, "#ifndef FUNC\n");
  str_append(self, "#define FUNC(name)\n");
  str_append(self, "#endif\n");
  str_append(self, "\n");
}

void str_append_meta_undefs(str *self) {
  str_append(self, "\n");
  str_append(self, "#undef ENUMERATOR\n");
  str_append(self, "#undef STRUCT\n");
  str_append(self, "#undef STRUCT_COND\n");
  str_append(self, "#undef STRUCT_COND_TRUE\n");
  str_append(self, "#undef STRUCT_COND_FALSE\n");
  str_append(self, "#undef FUNC\n");
  str_append(self, "\n");
}
void str_append_header_def(str *headerDefCode, platform_path *headerPath) {
  const char *relativeHeaderPath = platform_path_c_str(headerPath) + srcPath.data.size + 1;
  log_info("relative header path: %s", relativeHeaderPath);
  str_append(headerDefCode, "#if defined(ROOT)\n");
  str_append(headerDefCode, "#include \"");
  str_append(headerDefCode, relativeHeaderPath);
  str_append(headerDefCode, "\"\n");
  str_append(headerDefCode, "#elif defined(PARENT)\n");
  str_append(headerDefCode, "#include \"../");
  str_append(headerDefCode, relativeHeaderPath);
  str_append(headerDefCode, "\"\n");
  str_append(headerDefCode, "#endif\n");
}

// Scans enums in header and generates code.
void parse_header(platform_path *headerPath, str *structDefCode, str *headerDefCode) {
  log_info("header path: %s\n", platform_path_c_str(headerPath));
  char *input = read_text_file(headerPath, NULL);
  if (input == NULL) {
    panic("failed to read file %s!", str_c_str(&headerPath->data));
  }
  parser_state state = c_parser_execute(input);
  // parser_debug_print(&state);
  if (state.errors != NULL) {
    parser_debug_print(&state);
    panic("syntax errors in %s!", str_c_str(&headerPath->data));
  }

  str headerCode = str_init("");
  str sourceCode = str_init("");
  str_append_header_comment(&headerCode);
  str_append_source_comment(&sourceCode);

  // parse and generate code
  bool isVulkanHeader = platform_path_equals(headerPath, &vulkanHeaderPath);

  // add #include with relative header path to header.def
  if (!isVulkanHeader) {
    str_append_header_def(headerDefCode, headerPath);
  }
  c_parser_callbackData data = {&state, &headerCode, &sourceCode, structDefCode, isVulkanHeader};

  // parse enum declarations
  parser_ast_node_visit(state.programNode, enum_callback, &data);
  // parse structs
  parser_ast_node_visit(state.programNode, struct_callback, &data);
  // parse function declarations
  parser_ast_node_visit(state.programNode, function_declaration_callback, &data);

  // write generated header_code to file
  // log_debug("CODE:\n%s\n", str_c_str(&header_code));
  str basename = platform_path_get_basename(headerPath);
  platform_path headerOutputPath = platform_path_copy(&codegenPath);
  platform_path sourceOutputPath = platform_path_copy(&codegenPath);
  platform_path_append(&headerOutputPath, str_c_str(&basename));
  platform_path_append(&sourceOutputPath, str_c_str(&basename));
  str_free(&basename);
  sourceOutputPath.data.value[sourceOutputPath.data.size - 1] = 'c';
  log_info("header output: %s", str_c_str(&headerOutputPath.data));
  log_info("source output: %s", str_c_str(&sourceOutputPath.data));
  write_text_file(&headerOutputPath, &headerCode);
  write_text_file(&sourceOutputPath, &sourceCode);

  platform_path_free(&headerOutputPath);
  platform_path_free(&sourceOutputPath);
  parser_state_free(&state);
  free(input);
  str_free(&headerCode);
  str_free(&sourceCode);
}

int main(int argc, char *argv[]) {
  platform_init();
  log_info("codegen started");
  log_info("src path: %s", SRC_PATH);
  log_info("vulkan header path: %s", VULKAN_HEADER_PATH);

  srcPath = platform_path_init(SRC_PATH);
  vulkanHeaderPath = platform_path_init(VULKAN_HEADER_PATH);
  srcChildPathLst = get_dir_children(&srcPath);
  codegenPath = platform_path_copy(&srcPath);
  platform_path_append(&codegenPath, "codegen");
  if (!ignore_vulkan_header) {
    platform_path *vulkanHeaderPathCopy =
        (platform_path *)core_memdup(&vulkanHeaderPath, sizeof(platform_path));
    LL_APPEND(srcChildPathLst, vulkanHeaderPathCopy);
  }

  str structDefCode = str_init("");
  str headerDefCode = str_init("");
  str_append_meta_defines(&structDefCode);
  platform_path *childPath;
  LL_FOREACH(srcChildPathLst, childPath) {
    if (!platform_path_dirname_equals(childPath, &codegenPath)) {
      if (platform_path_ext_equals(childPath, ".h")) {
        parse_header(childPath, &structDefCode, &headerDefCode);
      }
    }
  }
  str_append_meta_undefs(&structDefCode);
  platform_path structDefPath = platform_path_copy(&codegenPath);
  platform_path_append(&structDefPath, "meta.def");
  write_text_file(&structDefPath, &structDefCode);

  platform_path headerDefPath = platform_path_copy(&codegenPath);
  platform_path_append(&headerDefPath, "header.def");
  write_text_file(&headerDefPath, &headerDefCode);

  platform_path_free(&headerDefPath);
  platform_path_free(&structDefPath);
  str_free(&headerDefCode);
  str_free(&structDefCode);
  platform_path_free(&codegenPath);
  platform_path_free(srcChildPathLst);
  platform_path_free(&srcPath);

  log_info("codegen finished");
  platform_free();
  return 0;
}
