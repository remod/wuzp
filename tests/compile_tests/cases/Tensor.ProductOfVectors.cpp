// wuzp-expect: error
// wuzp-static-assert: A tensor can only be multiplied by a scalar, a quantity or a tensor of rank 0\. Use innerProduct, outerProduct or cwiseProduct for other products\.

// The elementwise product of two vectors depends on the basis, so it is not a tensor. cwiseProduct computes it as quantities.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>>::zero() * TestTensor<Indices<Up>>::zero();
  return 0;
}
