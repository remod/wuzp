#include <gtest/gtest.h>

#include "test_compilation_error.hpp"

TEST(CompilationError, compilesGoodCode) {  // NOLINT
  ASSERT_NO_COMPILATION_ERROR(R"(
int main() {
  return 0;
}
)");
}

TEST(CompilationError, detectsBadCode) {  // NOLINT
  ASSERT_COMPILATION_ERROR(R"(
int main() {
  return 0
}
)");
}

TEST(CompilationError, detectsStaticAsserts) {  // NOLINT
  ASSERT_STATIC_ASSERTION_FAILURE(R"(
int main() {
  static_assert(false);
  return 0;
}
)");
}
