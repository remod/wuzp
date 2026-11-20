// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// boxMinus of active rotations in different coordinate systems has no meaning (#77).

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> rotationInA;
  const wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB> rotationInB;
  [[maybe_unused]] const auto difference = rotationInA.boxMinus(rotationInB);
  return 0;
}
