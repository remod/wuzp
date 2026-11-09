// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a multiplication\.

// Scaling a point depends on the origin. An affine combination is written p + (q - p) * t instead.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>, wuzp::Absolute>::zero() * 2.0;
  return 0;
}
