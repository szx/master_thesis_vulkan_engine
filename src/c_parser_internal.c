#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_parser_internal.h"

c_parser_state c_parser_state_init(char *source) {
  c_parser_state state = {};
  state.source = source;
  state.source_length = strlen(source);
  state.current = source;
  state.current_index = 0;
  state.errors = vec_c_parser_error_init();
  state.comments = vec_c_parser_comment_init();
  state.isValid = true;
  state.programNode = NULL;
  return state;
}

void c_parser_advance(c_parser_state *state, size_t n) {
  state->current += n;
  state->current_index += n;
}

char c_parser_peek(c_parser_state *state, size_t n) {
  // TODO: Probably doesn't work for really big documents.
  if (((int)state->current_index + n < 0) ||
      (state->current_index + n >= state->source_length)) {
    return -1;
  }
  return state->current[n];
}

char c_parser_getchar(c_parser_state *state) {
  char c = c_parser_peek(state, 0);
  c_parser_advance(state, 1);
  return c;
}

c_parser_str_range c_parser_str_range_init(size_t begin, size_t end) {
  c_parser_str_range range = {begin, end};
  return range;
}

c_parser_ast_node *c_parser_ast_node_allocate(c_parser_state *state,
                                              c_parser_ast_node_type type,
                                              c_parser_str_range range) {
  (void)state;
  c_parser_ast_node *node =
      (c_parser_ast_node *)malloc(sizeof(c_parser_ast_node));
  node->type = type;
  node->range = range;
  node->childNodes = lst_c_parser_ast_node_ptr_init();
  return node;
}

c_parser_ast_node *c_parser_ast_node_init_variadic(c_parser_state *state,
                                                   c_parser_ast_node_type type,
                                                   c_parser_str_range range) {
  c_parser_ast_node *node = c_parser_ast_node_allocate(state, type, range);
  return node;
}

void c_parser_ast_node_push_front(c_parser_ast_node *node,
                                  c_parser_ast_node *child_node) {
  lst_c_parser_ast_node_ptr_push_front(&node->childNodes,
                                       (c_parser_ast_node_ptr){child_node});
}

c_parser_ast_node *c_parser_ast_node_init_terminal(c_parser_state *state,
                                                   c_parser_ast_node_type type,
                                                   c_parser_str_range range) {
  c_parser_ast_node *node = c_parser_ast_node_allocate(state, type, range);
  return node;
}

c_parser_ast_node *c_parser_ast_node_init_unary(c_parser_state *state,
                                                c_parser_ast_node_type type,
                                                c_parser_str_range range,
                                                c_parser_ast_node *node1) {
  c_parser_ast_node *node = c_parser_ast_node_allocate(state, type, range);
  c_parser_ast_node_push_front(node, node1);
  return node;
}

c_parser_ast_node *c_parser_ast_node_init_binary(c_parser_state *state,
                                                 c_parser_ast_node_type type,
                                                 c_parser_str_range range,
                                                 c_parser_ast_node *node1,
                                                 c_parser_ast_node *node2) {
  c_parser_ast_node *node = c_parser_ast_node_allocate(state, type, range);
  c_parser_ast_node_push_front(node, node2);
  c_parser_ast_node_push_front(node, node1);
  return node;
}

void c_parser_ast_node_debug_print(c_parser_state *state,
                                   c_parser_ast_node *node,
                                   size_t indentLevel) {
  size_t len = node->range.end - node->range.begin;
  char *str = state->source + node->range.begin;
  printf("%*s| node %s (%d, %d): %.*s\n", (int)indentLevel, "",
         c_parser_ast_node_type_debug_str(node->type), (int)node->range.begin,
         (int)node->range.end, (int)len, str);
  size_t childNodeNum = 0;
  foreach (lst_c_parser_ast_node_ptr, &node->childNodes, it) {
    if (it.ref->node != NULL) {
      printf("%*s| child node %d:\n", (int)indentLevel, "",
             (int)++childNodeNum);
      c_parser_ast_node_debug_print(state, it.ref->node, indentLevel + 4);
    }
  }
}

void c_parser_handle_error(c_parser_state *state, c_parser_error_type type,
                           c_parser_str_range range) {
  c_parser_error error = {.type = type, .range = range};
  vec_c_parser_error_push_back(&state->errors, error);
  state->isValid = false;
}

void c_parser_handle_comment(c_parser_state *state, c_parser_str_range range) {
  c_parser_comment comment = {range};
  vec_c_parser_comment_push_back(&state->comments, comment);
}

void c_parser_debug_print(c_parser_state *state) {
  fprintf(stdout, "ERRORS:\n");
  for (size_t i = 0; i < state->errors.size; i++) {
    c_parser_error *error = &state->errors.value[i];
    if (error->type == c_parser_error_type_UnclosedComment) {
      fprintf(stdout, "ERROR: Unclosed comment block:\n");
    } else {
      fprintf(stdout, "ERROR: Undefined error:\n");
    }
    size_t len = error->range.end - error->range.begin;
    char *str = state->source + error->range.begin;
    fprintf(stdout, "       %.*s\n", (int)len, str);
  }
  fprintf(stdout, "COMMENTS:\n");
  for (size_t i = 0; i < state->comments.size; i++) {
    c_parser_comment *comment = &state->comments.value[i];
    size_t len = comment->range.end - comment->range.begin;
    char *str = state->source + comment->range.begin;
    fprintf(stdout, "COMMENT: %.*s\n", (int)len, str);
  }
  fprintf(stdout, "IS_VALID: %s\n", (state->isValid ? "true" : "false"));
  fprintf(stdout, "PROGRAM_NODE:\n");
  c_parser_ast_node_debug_print(state, state->programNode, 0);
}

const char *c_parser_ast_node_type_debug_str(c_parser_ast_node_type type) {
  switch (type) {
  case c_parser_ast_node_type_StatementList:
    return "StatementList";
  case c_parser_ast_node_type_String:
    return "String";
  case c_parser_ast_node_type_Identifier:
    return "Identifier";
  case c_parser_ast_node_type_Integer:
    return "Integer";
  case c_parser_ast_node_type_PreprocessorDirective:
    return "PreprocessorDirective";
  case c_parser_ast_node_type_SubscriptAccess:
    return "SubscriptAccess";
  case c_parser_ast_node_type_MemberAccess:
    return "MemberAccess";
  case c_parser_ast_node_type_PointerAccess:
    return "PointerAccess";
  case c_parser_ast_node_type_BitOr:
    return "BitOr";
  case c_parser_ast_node_type_BitAnd:
    return "BitAnd";
  case c_parser_ast_node_type_BitXor:
    return "BitXor";
  case c_parser_ast_node_type_BitShiftLeft:
    return "BitShiftLeft";
  case c_parser_ast_node_type_BitShiftRight:
    return "BitShiftRight";
  case c_parser_ast_node_type_BitNot:
    return "BitNot";
  case c_parser_ast_node_type_Not:
    return "Not";
  case c_parser_ast_node_type_Or:
    return "Or";
  case c_parser_ast_node_type_And:
    return "And";
  case c_parser_ast_node_type_Plus:
    return "Plus";
  case c_parser_ast_node_type_Minus:
    return "Minus";
  case c_parser_ast_node_type_Inc:
    return "Inc";
  case c_parser_ast_node_type_Dec:
    return "Dec";
  case c_parser_ast_node_type_Add:
    return "Add";
  case c_parser_ast_node_type_Sub:
    return "Sub";
  case c_parser_ast_node_type_Mul:
    return "Mul";
  case c_parser_ast_node_type_Div:
    return "Div";
  case c_parser_ast_node_type_Mod:
    return "Mod";
  case c_parser_ast_node_type_Assign:
    return "Assign";
  case c_parser_ast_node_type_AssignAdd:
    return "AssignAdd";
  case c_parser_ast_node_type_AssignSub:
    return "AssignSub";
  case c_parser_ast_node_type_AssignMul:
    return "AssignMul";
  case c_parser_ast_node_type_AssignDiv:
    return "AssignDiv";
  case c_parser_ast_node_type_AssignMod:
    return "AssignMod";
  case c_parser_ast_node_type_AssignBitOr:
    return "AssignBitOr";
  case c_parser_ast_node_type_AssignBitAnd:
    return "AssignBitAnd";
  case c_parser_ast_node_type_AssignBitXor:
    return "AssignBitXor";
  case c_parser_ast_node_type_AssignBitLeftShift:
    return "AssignBitLeftShift";
  case c_parser_ast_node_type_AssignBitRightShift:
    return "AssignBitRightShift";
  case c_parser_ast_node_type_EQ:
    return "EQ";
  case c_parser_ast_node_type_NE:
    return "NE";
  case c_parser_ast_node_type_LT:
    return "LT";
  case c_parser_ast_node_type_GT:
    return "GT";
  case c_parser_ast_node_type_LE:
    return "LE";
  case c_parser_ast_node_type_GE:
    return "GE";
  default:
    return "UNKNOWN";
  }
}
