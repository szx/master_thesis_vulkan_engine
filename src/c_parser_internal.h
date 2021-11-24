/* Structs and functions used by C parser/ */

#ifndef C_PARSER_INTERNAL_H
#define C_PARSER_INTERNAL_H

#include <stdbool.h>

typedef enum c_parser_ast_node_type {
  // lists
  c_parser_ast_node_type_TranslationUnit,
  c_parser_ast_node_type_LanguageLinkage,
  c_parser_ast_node_type_ArgumentExpressionList,
  c_parser_ast_node_type_ParameterList,
  c_parser_ast_node_type_EnumeratorDeclarationList,
  c_parser_ast_node_type_StructOrUnionDeclarationList,
  c_parser_ast_node_type_InitializerList,
  c_parser_ast_node_type_CompoundStatement,

  // terminals
  c_parser_ast_node_type_String,
  c_parser_ast_node_type_Identifier,
  c_parser_ast_node_type_MacroIdentifier,
  c_parser_ast_node_type_IntegerDec,
  c_parser_ast_node_type_IntegerOct,
  c_parser_ast_node_type_IntegerHex,
  c_parser_ast_node_type_TypeName,
  c_parser_ast_node_type_CompoundLiteral,

  // preprocessor directives
  c_parser_ast_node_type_PreprocessorDirective,
  c_parser_ast_node_type_PreprocessorDirectiveBody,
  c_parser_ast_node_type_PreprocessorMacroCall,

  // declarations
  c_parser_ast_node_type_Declaration, // Specifiers (Declarator = Initializer)
  c_parser_ast_node_type_DeclarationSpecifiers,    // Specifiers
  c_parser_ast_node_type_DeclaratorAndInitializer, // Declarator (= Initializer)
  c_parser_ast_node_type_DeclaratorAndInitializerList,
  c_parser_ast_node_type_IdentifierDeclarator,
  c_parser_ast_node_type_ArrayDeclarator,
  c_parser_ast_node_type_FunctionDeclaration,
  c_parser_ast_node_type_EnumerationDeclaration,
  c_parser_ast_node_type_EnumeratorDeclaration,
  c_parser_ast_node_type_StructDeclaration,
  c_parser_ast_node_type_UnionDeclaration,
  c_parser_ast_node_type_BitFieldDeclaration,
  c_parser_ast_node_type_TypedefStructDeclaration,
  c_parser_ast_node_type_TypedefUnionDeclaration,
  c_parser_ast_node_type_TypedefEnumDeclaration,
  c_parser_ast_node_type_TypedefTypeDeclaration,
  c_parser_ast_node_type_TypedefFunctionPointerDeclaration,
  c_parser_ast_node_type_FunctionPointerDeclarationSpecifiers,

  // definitions
  c_parser_ast_node_type_FunctionDefinition,

  // empty statement
  c_parser_ast_node_type_EmptyStatement,

  // labeled statements
  c_parser_ast_node_type_LabelStatement,
  c_parser_ast_node_type_CaseStatement,
  c_parser_ast_node_type_DefaultStatement,

  // jump statements
  c_parser_ast_node_type_ReturnStatement,
  c_parser_ast_node_type_ContinueStatement,
  c_parser_ast_node_type_BreakStatement,
  c_parser_ast_node_type_GotoStatement,

  // selection statements
  c_parser_ast_node_type_IfStatement,
  c_parser_ast_node_type_SwitchStatement,
  c_parser_ast_node_type_ForStatement,
  c_parser_ast_node_type_WhileStatement,
  c_parser_ast_node_type_DoWhileStatement,

  // access expression
  c_parser_ast_node_type_SubscriptAccess, // e[i]
  c_parser_ast_node_type_MemberAccess,    // e.i
  c_parser_ast_node_type_PointerAccess,   // e->i
  c_parser_ast_node_type_FunctionCall,    // e(i)

  // cast expression
  c_parser_ast_node_type_Cast, // (t)e

  // unary pointer expressions
  c_parser_ast_node_type_Deref, // *e
  c_parser_ast_node_type_Ref,   // &e

  // unary sizeof expression
  c_parser_ast_node_type_Sizeof, // sizeof e

  // binary bitwise operators
  c_parser_ast_node_type_BitOr,         // |
  c_parser_ast_node_type_BitAnd,        // &
  c_parser_ast_node_type_BitXor,        // ^
  c_parser_ast_node_type_BitShiftLeft,  // <<
  c_parser_ast_node_type_BitShiftRight, // >>

  // unary bitwise operators
  c_parser_ast_node_type_BitNot, // ~

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
  c_parser_ast_node_type_AssignBitAnd,        // &=
  c_parser_ast_node_type_AssignBitXor,        // ^=
  c_parser_ast_node_type_AssignBitLeftShift,  // <<=
  c_parser_ast_node_type_AssignBitRightShift, // >>=

  // logical operators
  c_parser_ast_node_type_Or,      // ||
  c_parser_ast_node_type_And,     // &&
  c_parser_ast_node_type_Not,     // !
  c_parser_ast_node_type_Ternary, // ?:

  // comparison operators
  c_parser_ast_node_type_EQ, // ==
  c_parser_ast_node_type_NE, // !=
  c_parser_ast_node_type_LT, // <
  c_parser_ast_node_type_GT, // >
  c_parser_ast_node_type_LE, // <=
  c_parser_ast_node_type_GE, // >=
} c_parser_ast_node_type;

typedef enum c_parser_error_type {
  c_parser_error_type_SyntaxError,
  c_parser_error_type_UnclosedComment,
  c_parser_error_type_UnclosedString,
  c_parser_error_type_MissingSemicolonAfterStatement
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
  char *source;         // null-terminated string
  size_t source_length; // length of source
  // Parsing state
  char *current;                 // current character in source
  size_t current_index;          // index of current character in source
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
c_parser_state c_parser_state_init(char *source);

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
                                  c_parser_ast_node *child_node);

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

// Returns debug string for AST node type.
const char *c_parser_ast_node_type_debug_str(c_parser_ast_node_type type);

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

#endif /* !C_PARSER_INTERNAL_H */
