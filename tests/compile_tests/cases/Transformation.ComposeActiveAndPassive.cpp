// wuzp-expect: error
// wuzp-static-assert: These transformations cannot be composed\.

// An active transformation, which moves objects within a coordinate system, and a passive one, which expresses them in another
// coordinate system, do not compose.

#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> active{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA> passive{1.0, 0.0, 0.0, 0.0, 1.0,
                                                                                                   0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = active * passive;
  return 0;
}
