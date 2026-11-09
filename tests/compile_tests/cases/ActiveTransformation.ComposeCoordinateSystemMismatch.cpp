// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Active transformations only compose within the same coordinate system.

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> rotationInA{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  const ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB> rotationInB{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = rotationInA * rotationInB;
  return 0;
}
