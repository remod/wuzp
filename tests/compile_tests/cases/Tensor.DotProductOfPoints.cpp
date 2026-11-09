// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a multiplication\.

// The dot product of two points depends on the origin, so it is not defined. The dot product of their difference, a vector,
// is.

#include "compile_test_types.hpp"

int main() {
  using Point = TestTensor<Indices<Up>, wuzp::Absolute>;
  [[maybe_unused]] const auto result = Point::zero().dotProduct(Point::zero());
  return 0;
}
