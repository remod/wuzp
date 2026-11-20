// wuzp-expect: error
// wuzp-static-assert: 'To' coordinate systems do not match\.

// The angle between passive rotations into different coordinate systems has no meaning (#77).

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> rotationAB;
  const wuzp::PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC> rotationAC;
  [[maybe_unused]] const auto angle = rotationAB.angularDistance(rotationAC);
  return 0;
}
