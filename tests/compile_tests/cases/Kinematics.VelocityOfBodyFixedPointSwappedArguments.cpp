// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// velocityOfBodyFixedPoint() takes the velocity, the angular velocity and the position vector, in this order (#76).

#include "wuzp/trafo/kinematics.hpp"

#include "compile_test_types.hpp"

int main() {
  using namespace wuzp;  // NOLINT
  const VelocityVector3<ValueMatrix, double, CoordinateSystemA> velocity{1.0, 2.0, 3.0};
  const AngularVelocityVector3<ValueMatrix, double, CoordinateSystemA> angularVelocity{4.0, 1.0, 2.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> offset{0.0, 0.0, 1.0};
  [[maybe_unused]] const auto velocityOfPoint = velocityOfBodyFixedPoint(velocity, offset, angularVelocity);
  return 0;
}
