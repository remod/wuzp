// wuzp-expect: error
// wuzp-error: no matching (member |overloaded )?function

// shift() takes the point to shift to as a coordinate system, a tag, not as a point with coordinates (#76).

#include "wuzp/trafo/SpatialForce.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialForceD<CoordinateSystemB, PointP, CoordinateSystemA> force{TorqueInA{0.0, 0.0, 1.0}, ForceInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto shifted =
      force.shift<wuzp::PositionPoint3<ValueMatrix, double, CoordinateSystemA>>(PositionInA{1.0, 0.0, 0.0});
  return 0;
}
