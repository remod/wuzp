// wuzp-expect: error
// wuzp-static-assert: 'From' coordinate systems do not match\.

// boxMinus of passive rotations between different coordinate systems has no meaning (#77).

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> rotationAB;
  const wuzp::PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemB> rotationCB;
  [[maybe_unused]] const auto difference = rotationAB.boxMinus(rotationCB);
  return 0;
}
