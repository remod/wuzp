// wuzp-expect: error
// wuzp-static-assert: A tensor can only be divided by a scalar, a quantity or a tensor of rank 0\. Use cwiseQuotient for elementwise quotients\.

// The elementwise quotient of two vectors depends on the basis, so it is not a tensor. cwiseQuotient computes it as quantities.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>>::zero() / TestTensor<Indices<Up>>::zero();
  return 0;
}
