// wuzp-expect: error
// wuzp-static-assert: Absolute/relative mismatch\.

// A rotation vector is a vector, not a point (#77).

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  using Rotation = wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const wuzp::HomogeneousPoint<3, ValueMatrix, double, Angle, CoordinateSystemA> point{0.1, 0.2, 0.3};
  [[maybe_unused]] const auto rotation = Rotation::fromRotationVector(point);
  return 0;
}
