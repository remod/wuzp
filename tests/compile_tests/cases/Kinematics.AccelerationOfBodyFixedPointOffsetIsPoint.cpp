// wuzp-expect: error
// wuzp-static-assert: Absolute/relative mismatch\.

// The position vector of accelerationOfBodyFixedPoint() is the difference of two points, not a point: the acceleration does not
// depend on the origin (#76).

#include "wuzp/trafo/kinematics.hpp"

#include "compile_test_types.hpp"

int main() {
  using namespace wuzp;  // NOLINT
  const AccelerationVector3<ValueMatrix, double, CoordinateSystemA> acceleration{1.0, 2.0, 3.0};
  const AngularAccelerationVector3<ValueMatrix, double, CoordinateSystemA> angularAcceleration{2.0, -1.0, 3.0};
  const AngularVelocityVector3<ValueMatrix, double, CoordinateSystemA> angularVelocity{4.0, 1.0, 2.0};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{0.0, 0.0, 1.0};
  [[maybe_unused]] const auto accelerationOfPoint = accelerationOfBodyFixedPoint(acceleration, angularAcceleration, angularVelocity, point);
  return 0;
}
