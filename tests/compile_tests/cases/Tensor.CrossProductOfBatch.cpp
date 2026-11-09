// wuzp-expect: error
// wuzp-static-assert: A batch of vectors, one per column, only supports transformations, sums, differences, comparisons, and products and quotients with scalars\.

// The cross product is defined for single vectors, not for batches of vectors.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestBatch<3>::zero().crossProduct(TestBatch<3>::zero());
  return 0;
}
