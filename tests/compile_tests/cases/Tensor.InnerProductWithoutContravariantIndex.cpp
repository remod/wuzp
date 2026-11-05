// wuzp-expect: error
// wuzp-static-assert: Total contravariant indices must be at least 1\.

// An inner product contracts a contravariant with a covariant index, so it needs at least one of each.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<0, 1>::zero().innerProduct(TestTensor<0, 1>::zero());
  return 0;
}
