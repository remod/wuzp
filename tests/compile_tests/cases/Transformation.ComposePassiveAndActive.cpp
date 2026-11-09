// wuzp-expect: error
// wuzp-static-assert: These transformations cannot be composed\.

// A passive transformation, which expresses objects in another coordinate system, and an active one, which moves them within a
// coordinate system, do not compose, in this order either.

#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> passive{1.0, 0.0, 0.0, 0.0, 1.0,
                                                                                                   0.0, 0.0, 0.0, 1.0};
  const ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> active{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = passive * active;
  return 0;
}
