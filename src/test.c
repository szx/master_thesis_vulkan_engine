#include "greatest.h"
#include "peg/c_parser.h"
#include "vulkan/vulkan.h"
#include <stdio.h>

// Basic test template.
TEST basic_test_template() { // NOLINT
  int x = 1;
  ASSERT_EQ(1, x);
  ASSERT_EQm("x should equal 1", 1, x);
  ASSERT_EQ_FMT(1, x, "%d");
  PASS();
}

SUITE(basic_test_suite) { RUN_TEST(basic_test_template); }

// Parsing C preprocessor directives.
TEST c_parser_preprocessor_parsing() { // NOLINT
  char *input = "2+2";
  c_parser_state state = {};
  state.source = input;
  state.source_length = sizeof(input);
  state.current = input;
  state.current_index = 0;
  c_parser_context_t *ctx = c_parser_create(&state);
  int ret;
  while (c_parser_parse(ctx, &ret)) {
  }
  c_parser_destroy(ctx);
  // TODO: Construct AST for preprocessor.
  int x = 1;
  ASSERT_EQ(1, x);
  ASSERT_EQm("x should equal 1", 1, x);
  ASSERT_EQ_FMT(1, x, "%d");
  PASS();
}

SUITE(c_parser_suite) { RUN_TEST(c_parser_preprocessor_parsing); }

GREATEST_MAIN_DEFS(); // NOLINT

int main(int argc, char *argv[]) {
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(basic_test_suite);
  RUN_SUITE(c_parser_suite);

  GREATEST_MAIN_END();
}
