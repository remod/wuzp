#include <gtest/gtest.h>

#include "test_compilation_error.hpp"

TEST(TestCompilationError, compilesGoodCode) {  // NOLINT
  ASSERT_NO_COMPILATION_ERROR(R"(
 int main() {
  return 0;
}
)");
}

TEST(TestCompilationError, detectsBadCode) {  // NOLINT
  ASSERT_COMPILATION_ERROR(R"(
int main() {
  return 0
}
)");
}

TEST(TestCompilationError, detectsStaticAsserts) {  // NOLINT
  ASSERT_STATIC_ASSERTION_FAILURE(R"(
 int main() {
  static_assert(false);
  return 0;
}
)");
}
