// wuzp-expect: error
// wuzp-static-assert: The outer product would have rank 3 or more, which is not supported\.

// The outer product of two rank 2 tensors would have rank 4, which is not supported.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::outerProduct(TestTensor<Indices<Up, Up>>::zero(), TestTensor<Indices<Up, Up>>::zero());
  return 0;
}
