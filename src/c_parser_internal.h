/* Structs and functions used by C parser/ */

#ifndef C_PARSER_INTERNAL_H
#define C_PARSER_INTERNAL_H

#include "platform.h"

typedef enum c_parser_ast_node_type {
  // lists
  TranslationUnit,
  LanguageLinkage,
  ArgumentExpressionList,
  ParameterList,
  EnumeratorDeclarationList,
  StructOrUnionDeclarationList,
  InitializerList,
  CompoundStatement,

  // terminals
  String,
  Identifier,
  MacroIdentifier,
  IntegerDec,
  IntegerOct,
  IntegerHex,
  TypeName,
  CompoundLiteral,

  // preprocessor directives
  PreprocessorDirective,
  PreprocessorDirectiveBody,
  PreprocessorMacroCall,

  // declarations
  Declaration,              // Specifiers (Declarator = Initializer)
  DeclarationSpecifiers,    // Specifiers
  DeclaratorAndInitializer, // Declarator (= Initializer)
  DeclaratorAndInitializerList,
  IdentifierDeclarator,
  ArrayDeclarator,
  FunctionDeclaration,
  EnumerationDeclaration,
  EnumeratorDeclaration,
  StructDeclaration,
  UnionDeclaration,
  BitFieldDeclaration,
  FunctionPointerDeclaration,
  TypedefStructDeclaration,
  TypedefUnionDeclaration,
  TypedefEnumDeclaration,
  TypedefTypeDeclaration,
  TypedefFunctionPointerDeclaration,
  FunctionPointerDeclarationSpecifiers,
  Ellipsis,

  // definitions
  FunctionDefinition,

  // empty statement
  EmptyStatement,

  // labeled statements
  LabelStatement,
  CaseStatement,
  DefaultStatement,

  // jump statements
  ReturnStatement,
  ContinueStatement,
  BreakStatement,
  GotoStatement,

  // selection statements
  IfStatement,
  SwitchStatement,
  ForStatement,
  WhileStatement,
  DoWhileStatement,

  // access expression
  SubscriptAccess, // e[i]
  MemberAccess,    // e.i
  PointerAccess,   // e->i
  FunctionCall,    // e(i)

  // cast expression
  Cast, // (t)e

  // unary pointer expressions
  Deref, // *e
  Ref,   // &e

  // unary sizeof expression
  Sizeof, // sizeof e

  // binary bitwise operators
  BitOr,         // |
  BitAnd,        // &
  BitXor,        // ^
  BitShiftLeft,  // <<
  BitShiftRight, // >>

  // unary bitwise operators
  BitNot, // ~

  // unary arithmetic operators
  Plus,  // +
  Minus, // -
  Inc,   // ++
  Dec,   // --

  // binary arithmetic operators
  Add, // +
  Sub, // -
  Mul, // *
  Div, // /
  Mod, // %

  // assignment operators
  Assign,              // ==
  AssignAdd,           // +=
  AssignSub,           // -=
  AssignMul,           // *=
  AssignDiv,           // /=
  AssignMod,           // %=
  AssignBitOr,         // |=
  AssignBitAnd,        // &=
  AssignBitXor,        // ^=
  AssignBitLeftShift,  // <<=
  AssignBitRightShift, // >>=

  // logical operators
  Or,      // ||
  And,     // &&
  Not,     // !
  Ternary, // ?:

  // comparison operators
  EQ, // ==
  NE, // !=
  LT, // <
  GT, // >
  LE, // <=
  GE, // >=
} c_parser_ast_node_type;

typedef enum c_parser_error_type {
  SyntaxError,
  UnclosedComment,
  UnclosedString,
  MissingSemicolonAfterStatement
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

typedef struct c_parser_ast_node c_parser_ast_node;

typedef struct c_parser_state {
  // Input state
  char *source;        // null-terminated string
  size_t sourceLength; // length of source
  // Parsing state
  char *current;                 // current character in source
  size_t currentIndex;           // index of current character in source
  vec_c_parser_error errors;     // every parsing error
  vec_c_parser_comment comments; // every parsed comment
  bool isValid; // false if detected remaining source text due to incompleteness
                // of grammar
  c_parser_ast_node *programNode; // parsed program AST node.
} c_parser_state;

typedef struct c_parser_ast_node_ptr {
  c_parser_ast_node *node;
} c_parser_ast_node_ptr;

#define P
#define T c_parser_ast_node_ptr
#include "lst.h" // lst_c_parser_ast_node_ptr

struct c_parser_ast_node {
  c_parser_ast_node_type type;
  c_parser_str_range range;
  lst_c_parser_ast_node_ptr childNodes;
};

// Returns new parser.
// Source becomes owned by parser.
c_parser_state c_parser_state_init(char *source);

// Frees parser and sets it to initial state.
void c_parser_state_free(c_parser_state *state);

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

// Returns variadic AST node.
c_parser_ast_node *c_parser_ast_node_init_variadic(c_parser_state *state,
                                                   c_parser_ast_node_type type,
                                                   c_parser_str_range range);

// Appends child AST node to AST node.
void c_parser_ast_node_push_front(c_parser_ast_node *node,
                                  c_parser_ast_node *childNode);

// Returns terminal AST node.
c_parser_ast_node *c_parser_ast_node_init_0(c_parser_state *state,
                                            c_parser_ast_node_type type,
                                            c_parser_str_range range);

// Returns unary AST node.
c_parser_ast_node *c_parser_ast_node_init_1(c_parser_state *state,
                                            c_parser_ast_node_type type,
                                            c_parser_str_range range,
                                            c_parser_ast_node *node1);

// Returns binary AST node.
c_parser_ast_node *c_parser_ast_node_init_2(c_parser_state *state,
                                            c_parser_ast_node_type type,
                                            c_parser_str_range range,
                                            c_parser_ast_node *node1,
                                            c_parser_ast_node *node2);

// Returns ternary AST node.
c_parser_ast_node *
c_parser_ast_node_init_3(c_parser_state *state, c_parser_ast_node_type type,
                         c_parser_str_range range, c_parser_ast_node *node1,
                         c_parser_ast_node *node2, c_parser_ast_node *node3);

// Returns quaternary AST node.
c_parser_ast_node *
c_parser_ast_node_init_4(c_parser_state *state, c_parser_ast_node_type type,
                         c_parser_str_range range, c_parser_ast_node *node1,
                         c_parser_ast_node *node2, c_parser_ast_node *node3,
                         c_parser_ast_node *node4);

// Visits every AST node.
void c_parser_ast_node_visit(c_parser_ast_node *node,
                             bool (*callback)(c_parser_ast_node *node,
                                              void *callbackData),
                             void *data);

// Prints node debug info to stdout.
void c_parser_ast_node_debug_print(c_parser_state *state,
                                   c_parser_ast_node *node, size_t indentLevel);

// Stores new parser error in c_parser_state.
void c_parser_handle_error(c_parser_state *state, c_parser_error_type type,
                           c_parser_str_range range);

// Stores new parser syntax error in c_parser_state.
// Used by PCC_ERROR() macro in PackCC parser.
void c_parser_handle_syntax_error(c_parser_state *state);

// Stores new comment in c_parser_state.
void c_parser_handle_comment(c_parser_state *state, c_parser_str_range range);

// Prints parser state to stdout.
void c_parser_debug_print(c_parser_state *state);

#include "codegen/c_parser_internal.h"

#endif /* !C_PARSER_INTERNAL_H */
