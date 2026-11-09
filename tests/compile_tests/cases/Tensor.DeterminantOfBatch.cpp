// wuzp-expect: error
// wuzp-static-assert: A batch of vectors, one per column, only supports transformations, sums, differences, comparisons, and products and quotients with scalars\.

// A batch of vectors is not a linear map, so it has no determinant.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestBatch<2>::zero().determinant();
  return 0;
}
