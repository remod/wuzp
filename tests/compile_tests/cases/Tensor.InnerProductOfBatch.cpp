// wuzp-expect: error
// wuzp-static-assert: A batch of vectors, one per column, only supports transformations, sums, differences, comparisons, and products and quotients with scalars\.

// The inner product contracts the index of a single vector, not of a batch of vectors.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::innerProduct(TestTensor<Indices<Down>>::zero(), TestBatch<1>::zero());
  return 0;
}
