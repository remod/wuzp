// wuzp-expect: error
// wuzp-static-assert: Cross product is only defined for three-dimensional vectors\.

// The skew-symmetric matrix is the matrix of the cross product, which is three-dimensional (#77).

#include "compile_test_types.hpp"

int main() {
  const wuzp::PositionVector2<ValueMatrix, double, CoordinateSystemA> position{1.0, 2.0};
  [[maybe_unused]] const auto skew = wuzp::skewSymmetricMatrix(position);
  return 0;
}
