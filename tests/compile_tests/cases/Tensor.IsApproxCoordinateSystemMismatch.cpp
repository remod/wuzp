// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Only tensors in the same coordinate system can be compared approximately.

#include "compile_test_types.hpp"

int main() {
  return TestTensor<1, 0>::zero().isApprox(TestTensor<1, 0, wuzp::Relative, CoordinateSystemB>::zero()) ? 0 : 1;
}
