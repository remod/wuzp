// wuzp-expect: error
// wuzp-static-assert: Contravariant indices do not match\.

// Only tensors with the same indices can be summed.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<1, 0>::zero() + TestTensor<0, 0>::zero();
  return 0;
}
