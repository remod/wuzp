// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a multiplication\.

// The cross product with a point, and so its skew-symmetric matrix, depends on the origin (#77).

#include "compile_test_types.hpp"

int main() {
  const wuzp::PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  [[maybe_unused]] const auto skew = wuzp::skewSymmetricMatrix(point);
  return 0;
}
