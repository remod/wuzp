// wuzp-expect: error
// wuzp-static-assert: 'GetCoordinateSystem' trait not specialized for given type\.

// A passive transformation first checks the coordinate system of the object. An object without one, e.g. a plain number,
// fails with a clear message.

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Rotation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;

int main() {
  const Rotation rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = rotation.transform(1.0);
  return 0;
}
