// wuzp-expect: error
// wuzp-static-assert: Indices do not match\.

// Only tensors of the same type can be compared approximately.

#include "compile_test_types.hpp"

int main() {
  return TestTensor<Indices<Up>>::zero().isApprox(TestTensor<Indices<>>::zero()) ? 0 : 1;
}
