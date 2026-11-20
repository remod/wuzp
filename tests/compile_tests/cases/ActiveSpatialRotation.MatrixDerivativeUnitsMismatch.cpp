// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// The derivative of a rotation matrix is computed from an angular velocity, not from a velocity (#77).

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> rotation;
  const wuzp::VelocityVector3<ValueMatrix, double, CoordinateSystemA> velocity{0.1, 0.2, 0.3};
  [[maybe_unused]] const auto derivative = rotation.matrixDerivative(velocity);
  return 0;
}
