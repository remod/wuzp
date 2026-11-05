// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// A passive transformation from A to B only accepts objects expressed in A.

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Rotation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
using PositionInC = PositionVector3<ValueMatrix, double, CoordinateSystemC>;

int main() {
  const Rotation rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  const PositionInC position{1.0, 2.0, 3.0};
  [[maybe_unused]] const auto result = rotation.transform(position);
  return 0;
}
