#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser_internal.h"

#include "../codegen/parser_internal.c"

void parser_comment_init(parser_comment *comment, parser_str_range range) {
  comment->range = range;
  comment->next = NULL;
}

void parser_comment_deinit(parser_comment *comment) {}

void parser_error_init(parser_error *error, parser_error_type type, parser_str_range range) {
  error->type = type;
  error->range = range;
  error->next = NULL;
}

void parser_error_deinit(parser_error *error) {}

parser_state parser_state_init(char *source) {
  parser_state state = {0};
  state.source = source;
  state.sourceLength = strlen(source);
  state.current = source;
  state.currentIndex = 0;
  state.errors = NULL;
  state.comments = NULL;
  state.isValid = true;
  state.programNode = NULL;
  return state;
}

void parser_state_free(parser_state *state) {
  state->source = NULL;
  state->sourceLength = 0;
  state->current = NULL;
  state->currentIndex = 0;
  parser_error *error, *tempError;
  LL_FOREACH_SAFE(state->errors,error,tempError) {
    LL_DELETE(state->errors,error);
    dealloc_struct(error);
  }
  parser_comment *comment, *tempComment;
  LL_FOREACH_SAFE(state->comments,comment,tempComment) {
    LL_DELETE(state->comments,comment);
    dealloc_struct(comment);
  }
  state->isValid = false;
  parser_ast_node_free(state->programNode);
  state->programNode = NULL;
}

void parser_advance(parser_state *state, size_t n) {
  state->current += n;
  state->currentIndex += n;
}

char parser_peek(parser_state *state, size_t n) {
  // TODO: Probably doesn't work for really big documents.
  if ((state->currentIndex + n >= state->sourceLength)) {
    return -1;
  }
  return state->current[n];
}

char parser_getchar(parser_state *state) {
  char c = parser_peek(state, 0);
  parser_advance(state, 1);
  return c;
}

parser_str_range parser_str_range_init(size_t begin, size_t end) {
  parser_str_range range = {begin, end};
  return range;
}

parser_ast_node *parser_ast_node_allocate(parser_state *state,
                                              parser_ast_node_type type,
                                              parser_str_range range) {
  (void)state;
  parser_ast_node *node =
      (parser_ast_node *)malloc(sizeof(parser_ast_node));
  node->type = type;
  node->range = range;
  node->childNodes = NULL;
  node->next = NULL;
  return node;
}

void parser_ast_node_free(parser_ast_node *node) {
  parser_ast_node *childNode, *temp;
  LL_FOREACH_SAFE(node->childNodes,childNode,temp) {
    if (childNode != NULL) {
      parser_ast_node_free(childNode);
    }
  }
  free(node);
}

parser_ast_node *parser_ast_node_init_variadic(parser_state *state,
                                                   parser_ast_node_type type,
                                                   parser_str_range range) {
  parser_ast_node *node = parser_ast_node_allocate(state, type, range);
  return node;
}

void parser_ast_node_push_front(parser_ast_node *node,
                                  parser_ast_node *childNode) {
  if (childNode == NULL) {
    return;
  }
  LL_PREPEND(node->childNodes, childNode);
}

parser_ast_node *parser_ast_node_init_0(parser_state *state,
                                            parser_ast_node_type type,
                                            parser_str_range range) {
  parser_ast_node *node = parser_ast_node_allocate(state, type, range);
  return node;
}

parser_ast_node *parser_ast_node_init_1(parser_state *state,
                                            parser_ast_node_type type,
                                            parser_str_range range,
                                            parser_ast_node *node1) {
  parser_ast_node *node = parser_ast_node_allocate(state, type, range);
  parser_ast_node_push_front(node, node1);
  return node;
}

parser_ast_node *parser_ast_node_init_2(parser_state *state,
                                            parser_ast_node_type type,
                                            parser_str_range range,
                                            parser_ast_node *node1,
                                            parser_ast_node *node2) {
  parser_ast_node *node = parser_ast_node_allocate(state, type, range);
  parser_ast_node_push_front(node, node2);
  parser_ast_node_push_front(node, node1);
  return node;
}

parser_ast_node *
parser_ast_node_init_3(parser_state *state, parser_ast_node_type type,
                         parser_str_range range, parser_ast_node *node1,
                         parser_ast_node *node2, parser_ast_node *node3) {
  parser_ast_node *node = parser_ast_node_allocate(state, type, range);
  parser_ast_node_push_front(node, node3);
  parser_ast_node_push_front(node, node2);
  parser_ast_node_push_front(node, node1);
  return node;
}

parser_ast_node *
parser_ast_node_init_4(parser_state *state, parser_ast_node_type type,
                         parser_str_range range, parser_ast_node *node1,
                         parser_ast_node *node2, parser_ast_node *node3,
                         parser_ast_node *node4) {
  parser_ast_node *node = parser_ast_node_allocate(state, type, range);
  parser_ast_node_push_front(node, node4);
  parser_ast_node_push_front(node, node3);
  parser_ast_node_push_front(node, node2);
  parser_ast_node_push_front(node, node1);
  return node;
}

char *parser_ast_node_c_str(parser_state *state, parser_ast_node *node) {
  size_t len = node->range.end - node->range.begin;
  char *str = state->source + node->range.begin;
  char *newStr = (char*)malloc((len+1)*sizeof(char*));
  sprintf(newStr, "%.*s", (int)len, str);
  return newStr;
}

size_t parser_ast_node_convert_int(parser_state *state, parser_ast_node *node) {
  char *newStr = parser_ast_node_c_str(state, node);
  size_t num;
  sscanf(newStr, "%zu", &num);
  free(newStr);
  return num;
}

void parser_ast_node_visit(parser_ast_node *node,
                             bool (*callback)(parser_ast_node *node,
                                              void *callbackData),
                             void *data) {
  bool result = callback(node, data);
  if (!result) {
    return;
  }
  parser_ast_node *childNode;
  LL_FOREACH(node->childNodes,childNode) {
    if (childNode != NULL) {
      parser_ast_node_visit(childNode, callback, data);
    }
  }
}

void parser_ast_node_debug_print(parser_state *state,
                                   parser_ast_node *node,
                                   size_t indentLevel) {
  size_t len = node->range.end - node->range.begin;
  char *str = state->source + node->range.begin;
  log_debug("%*s| node %s (%d, %d): %.*s\n", (int)indentLevel, "",
            parser_ast_node_type_debug_str(node->type),
            (int)node->range.begin, (int)node->range.end, (int)len, str);
  size_t childNodeNum = 0;
  parser_ast_node *childNode;
  LL_FOREACH(node->childNodes,childNode) {
    if (childNode != NULL) {
      log_debug("%*s| child node %d:\n", (int)indentLevel, "",
                (int)++childNodeNum);
      parser_ast_node_debug_print(state, childNode, indentLevel + 4);
    }
  }
}

void parser_handle_error(parser_state *state, parser_error_type type,
                           parser_str_range range) {
  parser_error *error = alloc_struct(parser_error);
  init_struct(error, parser_error_init, type, range);
  LL_APPEND(state->errors, error);
  state->isValid = false;
}

void parser_handle_syntax_error(parser_state *state) {
  static const size_t max_shown_chars = 100;
  parser_str_range range =
      parser_str_range_init(0, state->currentIndex + 1);
  if (state->currentIndex > max_shown_chars) {
    range.begin = state->currentIndex - max_shown_chars;
  }
  parser_error *error = alloc_struct(parser_error);
  init_struct(error, parser_error_init, SyntaxError, range);
  LL_APPEND(state->errors, error);
  state->isValid = false;
}

void parser_handle_comment(parser_state *state, parser_str_range range) {
  parser_comment *comment = alloc_struct(parser_comment);
  init_struct(comment, parser_comment_init, range);
  LL_APPEND(state->comments, comment);
}

void parser_debug_print(parser_state *state) {
  log_debug("ERRORS:\n");
  parser_error *error;
  LL_FOREACH(state->errors,error) {
    switch (error->type) {
    case SyntaxError: {
      log_debug("ERROR: Syntax error:\n");
    } break;
    case UnclosedComment: {
      log_debug("ERROR: Unclosed comment block:\n");
    } break;
    case UnclosedString: {
      log_debug("ERROR: Unclosed string literal:\n");
    } break;
    case MissingSemicolonAfterStatement: {
      log_debug("ERROR: Missing semicolon after expression:\n");
    } break;
    default: {
      log_debug("ERROR: Undefined error:\n");
    } break;
    }
    size_t len = error->range.end - error->range.begin;
    char *str = state->source + error->range.begin;
    log_debug("       %.*s\n", (int)len, str);
  }
  log_debug("COMMENTS:\n");
  parser_comment *comment;
  LL_FOREACH(state->comments,comment) {
    size_t len = comment->range.end - comment->range.begin;
    char *str = state->source + comment->range.begin;
    log_debug("COMMENT: %.*s\n", (int)len, str);
  }
  log_debug("IS_VALID: %s\n", (state->isValid ? "true" : "false"));
  log_debug("PROGRAM_NODE:\n");
  parser_ast_node_debug_print(state, state->programNode, 0);
}
