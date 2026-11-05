// wuzp-expect: error
// wuzp-static-assert: Covariant indices do not match\.

// Only tensors with the same indices can be subtracted.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<1, 0>::zero() - TestTensor<1, 1>::zero();
  return 0;
}
