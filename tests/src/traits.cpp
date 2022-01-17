#include <gtest/gtest.h>

#include "test_compilation_error.hpp"

TEST(TestTraits, AreSame) {  // NOLINT
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "wuzp/core/traits.hpp"

using namespace wuzp;  // NOLINT

using type = Product<double, int>::type;

int main() {
  return 0;
}
)", "'Product' trait not specialized for given type.")
}
