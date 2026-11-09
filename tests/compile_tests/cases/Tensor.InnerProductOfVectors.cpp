// wuzp-expect: error
// wuzp-static-assert: An inner product contracts an upper with a lower index, but the last index of the left-hand side and the first index of the right-hand side are both upper or both lower\.

// A contraction pairs an upper with a lower index, so it cannot contract two vectors. The dot product of two vectors needs a metric.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>>::zero().innerProduct(TestTensor<Indices<Up>>::zero());
  return 0;
}
