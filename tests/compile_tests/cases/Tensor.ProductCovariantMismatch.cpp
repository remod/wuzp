// wuzp-expect: error
// wuzp-static-assert: Covariant indices do not match\.

// The elementwise product requires tensors with the same indices.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<1, 0>::zero() * TestTensor<1, 1>::zero();
  return 0;
}
