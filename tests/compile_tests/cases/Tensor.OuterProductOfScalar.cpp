// wuzp-expect: error
// wuzp-static-assert: An outer product with a tensor of rank 0 is a product\. Multiply by it instead\.

// A tensor of rank 0 multiplies a tensor with *.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::outerProduct(TestTensor<Indices<Up>>::zero(), TestTensor<Indices<>>::zero());
  return 0;
}
