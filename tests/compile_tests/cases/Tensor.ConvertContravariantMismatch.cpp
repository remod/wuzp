// wuzp-expect: error
// wuzp-static-assert: Contravariant indices do not match\.

// A tensor cannot be converted into a tensor with different indices.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const TestTensor<1, 0> tensor = TestTensor<0, 0>::zero();
  return 0;
}
