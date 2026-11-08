// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Only tensors in the same coordinate system can be summed.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<1, 0>::zero() + TestTensor<1, 0, wuzp::Relative, CoordinateSystemB>::zero();
  return 0;
}
