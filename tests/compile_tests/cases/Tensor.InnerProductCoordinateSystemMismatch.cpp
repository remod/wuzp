// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// The inner product requires tensors in the same coordinate system.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result =
      wuzp::innerProduct(TestTensor<Indices<Down>>::zero(), TestTensor<Indices<Up>, wuzp::Relative, CoordinateSystemB>::zero());
  return 0;
}
