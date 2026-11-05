// wuzp-expect: error
// wuzp-static-assert: Covariant indices do not match\.

// Only tensors of the same type can be compared approximately.

#include "compile_test_types.hpp"

int main() {
  return TestTensor<1, 0>::zero().isApprox(TestTensor<1, 1>::zero()) ? 0 : 1;
}
