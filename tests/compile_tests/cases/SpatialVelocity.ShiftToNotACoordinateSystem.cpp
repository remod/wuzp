// wuzp-expect: error
// wuzp-error: no matching (member |overloaded )?function

// shift() takes the point to shift to as a coordinate system, a tag, not as a point with coordinates (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocity{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                   VelocityInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto shifted =
      velocity.shift<wuzp::PositionPoint3<ValueMatrix, double, CoordinateSystemA>>(PositionInA{1.0, 0.0, 0.0});
  return 0;
}
