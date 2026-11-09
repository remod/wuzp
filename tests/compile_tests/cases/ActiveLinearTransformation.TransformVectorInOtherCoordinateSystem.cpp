// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// An active linear transformation only transforms objects expressed in its coordinate system.

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Rotation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
using PositionInB = PositionVector3<ValueMatrix, double, CoordinateSystemB>;

int main() {
  const Rotation rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = rotation.transform(PositionInB{1.0, 2.0, 3.0});
  return 0;
}
