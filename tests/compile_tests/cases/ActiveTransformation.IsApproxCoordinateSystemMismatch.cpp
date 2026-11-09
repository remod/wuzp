// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Only transformations in the same coordinate system can be compared approximately.

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> rotationInA{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  const ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB> rotationInB{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  return rotationInA.isApprox(rotationInB) ? 0 : 1;
}
