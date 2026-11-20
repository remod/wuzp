// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// shift() takes the position vector from the point to the other point, not a force (#76).

#include "wuzp/trafo/SpatialForce.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialForceD<CoordinateSystemB, PointP, CoordinateSystemA> force{TorqueInA{0.0, 0.0, 1.0}, ForceInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto shifted = force.shift<PointQ>(ForceInA{1.0, 0.0, 0.0});
  return 0;
}
