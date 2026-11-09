// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// The outer product requires tensors in the same coordinate system.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result =
      wuzp::outerProduct(TestTensor<Indices<Up>>::zero(), TestTensor<Indices<Down>, wuzp::Relative, CoordinateSystemB>::zero());
  return 0;
}
