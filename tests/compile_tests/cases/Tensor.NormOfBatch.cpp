// wuzp-expect: error
// wuzp-static-assert: A batch of vectors, one per column, only supports transformations, sums, differences, comparisons, and products and quotients with scalars\.

// The norm is defined for a single vector, not for a batch of vectors.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestBatch<3>::zero().norm();
  return 0;
}
