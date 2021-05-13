#include <gtest/gtest.h>

#include "test_compilation_error.hpp"

TEST(CompilationError, compilesGoodCode) {  // NOLINT
  ASSERT_NO_COMPILATION_ERROR(R"(
int main() {
  return 0;
}
)");
}
