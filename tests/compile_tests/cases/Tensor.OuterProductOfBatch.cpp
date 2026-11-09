// wuzp-expect: error
// wuzp-static-assert: A batch of vectors, one per column, only supports transformations, sums, differences, comparisons, and products and quotients with scalars\.

// The outer product is defined for a single vector, not for a batch of vectors.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::outerProduct(TestBatch<3>::zero(), TestTensor<Indices<Up>>::zero());
  return 0;
}
