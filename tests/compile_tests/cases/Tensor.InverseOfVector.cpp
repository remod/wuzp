// wuzp-expect: error
// wuzp-static-assert: A vector or a covector has no inverse\. Use cwiseInverse\(\) for the reciprocals of its components\.

// Only tensors of rank 0 and 2 have a matrix inverse, also in one dimension, where the components of a vector are a 1x1
// matrix.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>>::zero().inverse();
  return 0;
}
