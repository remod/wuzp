// wuzp-expect: error
// wuzp-static-assert: Covariant indices do not match\.

// A tensor cannot be converted into a tensor with different indices.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const TestTensor<1, 1> tensor{TestTensor<1, 0>::zero()};
  return 0;
}
