// wuzp-expect: error
// wuzp-static-assert: A batch of vectors, one per column, only supports transformations, sums, differences, comparisons, and products and quotients with scalars\.

// The elementwise reciprocal swaps and flips the indices, which is defined for a single vector, not for a batch.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestBatch<3>::zero().inverse();
  return 0;
}
