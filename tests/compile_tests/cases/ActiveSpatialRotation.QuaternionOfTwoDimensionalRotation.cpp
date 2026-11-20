// wuzp-expect: error
// wuzp-static-assert: Quaternions, angle-axis, rotation vectors, Euler angles, boxPlus, boxMinus and the derivatives of rotations are only defined for three-dimensional rotations\.

// Quaternions parameterize three-dimensional rotations (#27).

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::ActiveSpatialRotation2<ValueMatrix, double, CoordinateSystemA> rotation;
  [[maybe_unused]] const auto quaternion = rotation.toQuaternion();
  return 0;
}
