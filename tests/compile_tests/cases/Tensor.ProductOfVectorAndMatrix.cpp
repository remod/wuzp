// wuzp-expect: error
// wuzp-static-assert: A tensor can only be multiplied by a scalar, a quantity or a tensor of rank 0\. Use innerProduct, outerProduct or cwiseProduct for other products\.

// A tensor of rank 2 does not multiply a vector with *, which would be elementwise: innerProduct contracts their indices.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up, Down>>::zero() * TestTensor<Indices<Up>>::zero();
  return 0;
}
