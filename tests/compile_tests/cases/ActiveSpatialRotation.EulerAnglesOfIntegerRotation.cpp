// wuzp-expect: error
// wuzp-static-assert: The parameterizations, the manifold operations and the derivatives of rotations need floating-point values\.

// A rotation with integer values, e.g. by multiples of pi/2, has no Euler angles, which are not integers in general (#77).

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::ActiveSpatialRotation3<ValueMatrix, int, CoordinateSystemA> rotation;
  [[maybe_unused]] const auto angles = rotation.toEulerAnglesZyx();
  return 0;
}
