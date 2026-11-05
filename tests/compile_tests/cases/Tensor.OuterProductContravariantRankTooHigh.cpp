// wuzp-expect: error
// wuzp-static-assert: Tensors with rank 3 or more are not supported\. Smaller contravariant incides must be at most 1\.

// The outer product of two rank 2 tensors would have rank 4, which is not supported.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::outerProduct(TestTensor<2, 0>::zero(), TestTensor<2, 0>::zero());
  return 0;
}
