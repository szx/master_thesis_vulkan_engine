#include "calc.h"
#include "greatest.h"
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

GREATEST_MAIN_DEFS(); // NOLINT

int main(int argc, char *argv[]) {
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(basic_test_suite);

  GREATEST_MAIN_END();
}
