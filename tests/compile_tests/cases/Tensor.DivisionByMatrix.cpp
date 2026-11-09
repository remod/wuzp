// wuzp-expect: error
// wuzp-static-assert: A tensor can only be divided by a scalar, a quantity or a tensor of rank 0\. Use cwiseQuotient for elementwise quotients\.

// A tensor can only be divided by a scalar, not by a tensor of rank 2.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<>>::zero() / TestTensor<Indices<Up, Down>>::zero();
  return 0;
}
