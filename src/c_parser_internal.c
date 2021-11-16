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
  node->node1 = NULL;
  node->node2 = NULL;
  return node;
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
  node->node1 = node1;
  return node;
}

c_parser_ast_node *c_parser_ast_node_init_binary(c_parser_state *state,
                                                 c_parser_ast_node_type type,
                                                 c_parser_str_range range,
                                                 c_parser_ast_node *node1,
                                                 c_parser_ast_node *node2) {
  c_parser_ast_node *node = c_parser_ast_node_allocate(state, type, range);
  node->node1 = node1;
  node->node2 = node2;
  return node;
}

void c_parser_ast_node_debug_print(c_parser_state *state,
                                   c_parser_ast_node *node,
                                   size_t indentLevel) {
  size_t len = node->range.end - node->range.begin;
  char *str = state->source + node->range.begin;
  printf("%*s| node %d (%d, %d): %.*s\n", (int)indentLevel, "", (int)node->type,
         (int)node->range.begin, (int)node->range.end, (int)len, str);
  if (node->node1 != NULL) {
    printf("%*s| node1:\n", (int)indentLevel, "");
    c_parser_ast_node_debug_print(state, node->node1, indentLevel + 4);
  }
  if (node->node2 != NULL) {
    printf("%*s| node2:\n", (int)indentLevel, "");
    c_parser_ast_node_debug_print(state, node->node2, indentLevel + 4);
  }
}

void c_parser_handle_error(c_parser_state *state, c_parser_error_type type,
                           c_parser_str_range range) {
  c_parser_error error = {.type = type, .range = range};
  vec_c_parser_error_push_back(&state->errors, error);
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
}
