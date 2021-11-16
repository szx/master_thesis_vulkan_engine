/* Structs and functions used by C parser/ */

#ifndef C_PARSER_INTERNAL_H
#define C_PARSER_INTERNAL_H

typedef enum c_parser_ast_node_type {
  c_parser_ast_node_type_String,     // STRING
  c_parser_ast_node_type_Identifier, // IDENTIFIER
  c_parser_ast_node_type_Integer,    // NUMBER

  c_parser_ast_node_type_PreprocessorDirective, // #directive

  // bitwise operators
  c_parser_ast_node_type_BitOr,         // |
  c_parser_ast_node_type_BitAnd,        // &
  c_parser_ast_node_type_BitXor,        // ^
  c_parser_ast_node_type_BitShiftLeft,  // <<
  c_parser_ast_node_type_BitShiftRight, // >>
  c_parser_ast_node_type_BitNot,        // ~

  // logical operators
  c_parser_ast_node_type_Not, // !
  c_parser_ast_node_type_Or,  // ||
  c_parser_ast_node_type_And, // &&

  // unary arithmetic operators
  c_parser_ast_node_type_Plus,  // +
  c_parser_ast_node_type_Minus, // -
  c_parser_ast_node_type_Inc,   // ++
  c_parser_ast_node_type_Dec,   // --

  // binary arithmetic operators
  c_parser_ast_node_type_Add, // +
  c_parser_ast_node_type_Sub, // -
  c_parser_ast_node_type_Mul, // *
  c_parser_ast_node_type_Div, // /
  c_parser_ast_node_type_Mod, // %

  // assignment operators
  c_parser_ast_node_type_Assign,              // ==
  c_parser_ast_node_type_AssignAdd,           // +=
  c_parser_ast_node_type_AssignSub,           // -=
  c_parser_ast_node_type_AssignMul,           // *=
  c_parser_ast_node_type_AssignDiv,           // /=
  c_parser_ast_node_type_AssignMod,           // %=
  c_parser_ast_node_type_AssignBitOr,         // |=
  c_parser_ast_node_type_AssignBitAnd,        // %=
  c_parser_ast_node_type_AssignBitXor,        // ^=
  c_parser_ast_node_type_AssignBitLeftShift,  // <<=
  c_parser_ast_node_type_AssignBitRightShift, // >>=

  // comparison operators
  c_parser_ast_node_type_EQ, // ==
  c_parser_ast_node_type_NE, // !=
  c_parser_ast_node_type_LT, // <
  c_parser_ast_node_type_GT, // >
  c_parser_ast_node_type_LE, // <=
  c_parser_ast_node_type_GE, // >=
} c_parser_ast_node_type;

typedef enum c_parser_error_type {
  c_parser_error_type_UnclosedComment
} c_parser_error_type;

typedef struct c_parser_str_range {
  size_t begin; // inclusive
  size_t end;   // exclusive
} c_parser_str_range;

typedef struct c_parser_error {
  c_parser_error_type type;
  c_parser_str_range range;
} c_parser_error;
#define P
#define T c_parser_error
#include "vec.h" // vec_c_parser_error

typedef struct c_parser_comment {
  c_parser_str_range range;
} c_parser_comment;
#define P
#define T c_parser_comment
#include "vec.h" // vec_c_parser_comment

typedef struct c_parser_state {
  // Input state
  char *source;         // null-terminated string
  size_t source_length; // length of source
  // Parsing state
  char *current;                 // current character in source
  size_t current_index;          // index of current character in source
  vec_c_parser_error errors;     // every parsing error
  vec_c_parser_comment comments; // every parsed comment
} c_parser_state;

typedef struct c_parser_ast_node {
  c_parser_ast_node_type type;
  c_parser_str_range range;
  struct c_parser_ast_node *node1; // first child node
  struct c_parser_ast_node *node2; // second child node
} c_parser_ast_node;

// Advances by 'n' characters.
void c_parser_advance(c_parser_state *state, size_t n);

// Returns next (or previous) n-th character.
char c_parser_peek(c_parser_state *state, size_t n);

// Peeks current character and advances by 1 character.
char c_parser_getchar(c_parser_state *state);

// Returns new string range.
c_parser_str_range c_parser_str_range_init(size_t begin, size_t end);

// Returns newly allocated AST node.
c_parser_ast_node *c_parser_ast_node_allocate(c_parser_state *state,
                                              c_parser_ast_node_type type,
                                              c_parser_str_range range);

// Returns terminal AST node.
c_parser_ast_node *c_parser_ast_node_init_terminal(c_parser_state *state,
                                                   c_parser_ast_node_type type,
                                                   c_parser_str_range range);

// Returns unary AST node.
c_parser_ast_node *c_parser_ast_node_init_unary(c_parser_state *state,
                                                c_parser_ast_node_type type,
                                                c_parser_str_range range,
                                                c_parser_ast_node *node1);

// Returns binary AST node.
c_parser_ast_node *c_parser_ast_node_init_binary(c_parser_state *state,
                                                 c_parser_ast_node_type type,
                                                 c_parser_str_range range,
                                                 c_parser_ast_node *node1,
                                                 c_parser_ast_node *node2);

// Prints node debug info to stdout.
void c_parser_ast_node_debug_print(c_parser_state *state,
                                   c_parser_ast_node *node, size_t indentLevel);

// Stores new parser error in c_parser_state.
void c_parser_handle_error(c_parser_state *state, c_parser_error_type type,
                           c_parser_str_range range);

// Stores new comment in c_parser_state.
void c_parser_handle_comment(c_parser_state *state, c_parser_str_range range);

// Prints parser state to stdout.
void c_parser_debug_print(c_parser_state *state);

#endif /* !C_PARSER_INTERNAL_H */
