// wuzp-expect: error
// wuzp-static-assert: Indices do not match\.

// The dot product requires tensors with the same indices. A vector and a covector have the same number of indices, but not the same
// indices.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>>::zero().dotProduct(TestTensor<Indices<Down>>::zero());
  return 0;
}
