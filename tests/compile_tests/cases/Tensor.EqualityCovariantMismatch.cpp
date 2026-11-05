// wuzp-expect: error
// wuzp-static-assert: Covariant indices do not match\.

// Only tensors with the same indices can be compared.

#include "compile_test_types.hpp"

int main() {
  return TestTensor<1, 0>::zero() == TestTensor<1, 1>::zero() ? 0 : 1;
}
