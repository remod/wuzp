// wuzp-expect: error
// wuzp-static-assert: Indices do not match\.

// Only tensors with the same indices can be compared.

#include "compile_test_types.hpp"

int main() {
  return TestTensor<Indices<Up>>::zero() == TestTensor<Indices<>>::zero() ? 0 : 1;
}
