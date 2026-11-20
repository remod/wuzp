// wuzp-expect: error
// wuzp-static-assert: The parameterizations, the manifold operations and the derivatives of rotations need floating-point values\.

// A passive rotation with integer values, e.g. by multiples of pi/2, has no rotation vector, which is not an integer vector in
// general (#77).

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::PassiveSpatialRotation3<ValueMatrix, int, CoordinateSystemA, CoordinateSystemB> rotation;
  [[maybe_unused]] const auto rotationVector = rotation.toRotationVector();
  return 0;
}
