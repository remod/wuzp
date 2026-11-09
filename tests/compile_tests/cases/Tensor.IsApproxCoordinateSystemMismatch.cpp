// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Only tensors in the same coordinate system can be compared approximately.

#include "compile_test_types.hpp"

int main() {
  return TestTensor<Indices<Up>>::zero().isApprox(TestTensor<Indices<Up>, wuzp::Relative, CoordinateSystemB>::zero()) ? 0 : 1;
}
