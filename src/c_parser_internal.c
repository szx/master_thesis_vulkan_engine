#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_parser_internal.h"

void c_parser_ast_node_free(c_parser_ast_node *pNode);
c_parser_state c_parser_state_init(char *source) {
  c_parser_state state = {0};
  state.source = source;
  state.sourceLength = strlen(source);
  state.current = source;
  state.currentIndex = 0;
  state.errors = vec_c_parser_error_init();
  state.comments = vec_c_parser_comment_init();
  state.isValid = true;
  state.programNode = NULL;
  return state;
}

void c_parser_state_free(c_parser_state *state) {
  free(state->source);
  state->source = NULL;
  state->sourceLength = 0;
  state->current = NULL;
  state->currentIndex = 0;
  vec_c_parser_error_free(&state->errors);
  vec_c_parser_comment_free(&state->comments);
  state->isValid = false;
  c_parser_ast_node_free(state->programNode);
  state->programNode = NULL;
}

void c_parser_advance(c_parser_state *state, size_t n) {
  state->current += n;
  state->currentIndex += n;
}

char c_parser_peek(c_parser_state *state, size_t n) {
  // TODO: Probably doesn't work for really big documents.
  if ((state->currentIndex + n >= state->sourceLength)) {
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

void c_parser_ast_node_free(c_parser_ast_node *node) {
  foreach (lst_c_parser_ast_node_ptr, &node->childNodes, it) {
    if (it.ref->node != NULL) {
      c_parser_ast_node_free(it.ref->node);
    }
  }
  lst_c_parser_ast_node_ptr_free(&node->childNodes);
  free(node);
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

c_parser_ast_node *c_parser_ast_node_init_0(c_parser_state *state,
                                            c_parser_ast_node_type type,
                                            c_parser_str_range range) {
  c_parser_ast_node *node = c_parser_ast_node_allocate(state, type, range);
  return node;
}

c_parser_ast_node *c_parser_ast_node_init_1(c_parser_state *state,
                                            c_parser_ast_node_type type,
                                            c_parser_str_range range,
                                            c_parser_ast_node *node1) {
  c_parser_ast_node *node = c_parser_ast_node_allocate(state, type, range);
  c_parser_ast_node_push_front(node, node1);
  return node;
}

c_parser_ast_node *c_parser_ast_node_init_2(c_parser_state *state,
                                            c_parser_ast_node_type type,
                                            c_parser_str_range range,
                                            c_parser_ast_node *node1,
                                            c_parser_ast_node *node2) {
  c_parser_ast_node *node = c_parser_ast_node_allocate(state, type, range);
  c_parser_ast_node_push_front(node, node2);
  c_parser_ast_node_push_front(node, node1);
  return node;
}

c_parser_ast_node *
c_parser_ast_node_init_3(c_parser_state *state, c_parser_ast_node_type type,
                         c_parser_str_range range, c_parser_ast_node *node1,
                         c_parser_ast_node *node2, c_parser_ast_node *node3) {
  c_parser_ast_node *node = c_parser_ast_node_allocate(state, type, range);
  c_parser_ast_node_push_front(node, node3);
  c_parser_ast_node_push_front(node, node2);
  c_parser_ast_node_push_front(node, node1);
  return node;
}

c_parser_ast_node *
c_parser_ast_node_init_4(c_parser_state *state, c_parser_ast_node_type type,
                         c_parser_str_range range, c_parser_ast_node *node1,
                         c_parser_ast_node *node2, c_parser_ast_node *node3,
                         c_parser_ast_node *node4) {
  c_parser_ast_node *node = c_parser_ast_node_allocate(state, type, range);
  c_parser_ast_node_push_front(node, node4);
  c_parser_ast_node_push_front(node, node3);
  c_parser_ast_node_push_front(node, node2);
  c_parser_ast_node_push_front(node, node1);
  return node;
}

void c_parser_ast_node_visit(c_parser_ast_node *node,
                             bool (*callback)(c_parser_ast_node *node,
                                              void *callback_data),
                             void *data) {
  bool result = callback(node, data);
  if (!result) {
    return;
  }
  foreach (lst_c_parser_ast_node_ptr, &node->childNodes, it) {
    if (it.ref->node != NULL) {
      c_parser_ast_node_visit(it.ref->node, callback, data);
    }
  }
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

void c_parser_handle_syntax_error(c_parser_state *state) {
  static const size_t max_shown_chars = 100;
  c_parser_str_range range =
      c_parser_str_range_init(0, state->currentIndex + 1);
  if (state->currentIndex > max_shown_chars) {
    range.begin = state->currentIndex - max_shown_chars;
  }
  c_parser_error error = {.type = SyntaxError, .range = range};
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
    switch (error->type) {
    case SyntaxError: {
      fprintf(stdout, "ERROR: Syntax error:\n");
    } break;
    case UnclosedComment: {
      fprintf(stdout, "ERROR: Unclosed comment block:\n");
    } break;
    case UnclosedString: {
      fprintf(stdout, "ERROR: Unclosed string literal:\n");
    } break;
    case MissingSemicolonAfterStatement: {
      fprintf(stdout, "ERROR: Missing semicolon after expression:\n");
    } break;
    default: {
      fprintf(stdout, "ERROR: Undefined error:\n");
    } break;
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

#include "codegen/c_parser_internal.h"
