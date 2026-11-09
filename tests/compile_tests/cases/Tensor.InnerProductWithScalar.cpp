// wuzp-expect: error
// wuzp-static-assert: An inner product contracts an index of each tensor, but a tensor of rank 0 has none\. Multiply by it instead\.

// A tensor of rank 0 has no index to contract, also on the right-hand side: it multiplies a tensor with *.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::innerProduct(TestTensor<Indices<Up>>::zero(), TestTensor<Indices<>>::zero());
  return 0;
}
