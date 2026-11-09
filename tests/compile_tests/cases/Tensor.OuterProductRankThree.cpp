// wuzp-expect: error
// wuzp-static-assert: The outer product would have rank 3 or more, which is not supported\.

// The outer product of a covector and a rank 2 tensor would have rank 3, which is not supported.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::outerProduct(TestTensor<Indices<Down>>::zero(), TestTensor<Indices<Down, Down>>::zero());
  return 0;
}
