/* Structs and functions used by C parser */

#ifndef PARSER_INTERNAL_H
#define PARSER_INTERNAL_H

#include "../core/platform.h"

typedef enum parser_ast_node_type {
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

  // static assert
  StaticAssert,

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
  UnnamedStructDeclaration,
  UnionDeclaration,
  UnnamedUnionDeclaration,
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

  // GLSL
  VertexInputAttribute,
  VertexOutputAttribute,
  PushConstant,
  UniformBlock,
  VariableDeclaration,
  VectorType,
  MatrixType
} parser_ast_node_type;

typedef enum parser_error_type {
  SyntaxError,
  UnclosedComment,
  UnclosedString,
  MissingSemicolonAfterStatement
} parser_error_type;

typedef struct parser_str_range {
  size_t begin; // inclusive
  size_t end;   // exclusive
} parser_str_range;

typedef struct parser_error {
  parser_error_type type;
  parser_str_range range;
  struct parser_error *next;
} parser_error;

parser_error *parser_error_create(parser_error_type type, parser_str_range range);
void parser_error_destroy(parser_error *error);

typedef struct parser_comment {
  parser_str_range range;
  struct parser_comment *next;
} parser_comment;

parser_comment *parser_comment_create(parser_str_range range);
void parser_comment_destroy(parser_comment *comment);

typedef struct parser_ast_node parser_ast_node;

typedef struct parser_state {
  // Input state
  char *source;        // null-terminated string
  size_t sourceLength; // length of source
  // Parsing state
  char *current;                // current character in source
  size_t currentIndex;          // index of current character in source
  parser_error *errors;       // every parsing error
  parser_comment *comments;     // every parsed comment
  bool isValid; // false if detected remaining source text due to incompleteness
                // of grammar
  parser_ast_node *programNode; // parsed program AST node.
} parser_state;

struct parser_ast_node {
  parser_ast_node_type type;
  parser_str_range range;
  parser_ast_node *childNodes;
  struct parser_ast_node *next; // sibling
};

void parser_ast_node_free(parser_ast_node *node);

// Returns new parser.
// Source becomes owned by parser.
parser_state parser_state_init(char *source);

// Frees parser and sets it to initial state.
void parser_state_free(parser_state *state);

// Advances by 'n' characters.
void parser_advance(parser_state *state, size_t n);

// Returns next (or previous) n-th character.
char parser_peek(parser_state *state, size_t n);

// Peeks current character and advances by 1 character.
char parser_getchar(parser_state *state);

// Returns new string range.
parser_str_range parser_str_range_init(size_t begin, size_t end);

// Returns newly allocated AST node.
parser_ast_node *parser_ast_node_allocate(parser_state *state,
                                              parser_ast_node_type type,
                                              parser_str_range range);

// Returns variadic AST node.
parser_ast_node *parser_ast_node_init_variadic(parser_state *state,
                                                   parser_ast_node_type type,
                                                   parser_str_range range);

// Appends child AST node to AST node.
void parser_ast_node_push_front(parser_ast_node *node,
                                  parser_ast_node *childNode);

// Returns terminal AST node.
parser_ast_node *parser_ast_node_init_0(parser_state *state,
                                            parser_ast_node_type type,
                                            parser_str_range range);

// Returns unary AST node.
parser_ast_node *parser_ast_node_init_1(parser_state *state,
                                            parser_ast_node_type type,
                                            parser_str_range range,
                                            parser_ast_node *node1);

// Returns binary AST node.
parser_ast_node *parser_ast_node_init_2(parser_state *state,
                                            parser_ast_node_type type,
                                            parser_str_range range,
                                            parser_ast_node *node1,
                                            parser_ast_node *node2);

// Returns ternary AST node.
parser_ast_node *
parser_ast_node_init_3(parser_state *state, parser_ast_node_type type,
                         parser_str_range range, parser_ast_node *node1,
                         parser_ast_node *node2, parser_ast_node *node3);

// Returns quaternary AST node.
parser_ast_node *
parser_ast_node_init_4(parser_state *state, parser_ast_node_type type,
                         parser_str_range range, parser_ast_node *node1,
                         parser_ast_node *node2, parser_ast_node *node3,
                         parser_ast_node *node4);

// Allocates new C string.
char *parser_ast_node_c_str(parser_state *state, parser_ast_node *node);

// Convert node to integer.
size_t parser_ast_node_convert_int(parser_state *state, parser_ast_node *node);

// Visits every AST node.
void parser_ast_node_visit(parser_ast_node *node,
                             bool (*callback)(parser_ast_node *node,
                                              void *callbackData),
                             void *data);

// Prints node debug info to stdout.
void parser_ast_node_debug_print(parser_state *state,
                                   parser_ast_node *node, size_t indentLevel);

// Stores new parser error in parser_state.
void parser_handle_error(parser_state *state, parser_error_type type,
                           parser_str_range range);

// Stores new parser syntax error in parser_state.
// Used by PCC_ERROR() macro in PackCC parser.
void parser_handle_syntax_error(parser_state *state);

// Stores new comment in parser_state.
void parser_handle_comment(parser_state *state, parser_str_range range);

// Prints parser state to stdout.
void parser_debug_print(parser_state *state);

#include "../codegen/parser_internal.h"

#endif /* !PARSER_INTERNAL_H */
