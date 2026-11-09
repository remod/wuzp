// wuzp-expect: error
// wuzp-static-assert: Indices do not match\.

// Only tensors with the same indices can be summed.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>>::zero() + TestTensor<Indices<>>::zero();
  return 0;
}
