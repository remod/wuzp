// wuzp-expect: error
// wuzp-static-assert: Only a tensor with one upper index, a vector or a batch of vectors, can be absolute \(a point\)\.

// A covector pairs with vectors, not with points, so there are no absolute covectors.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(TestTensor<Indices<Down>, wuzp::Absolute>);
  return 0;
}
