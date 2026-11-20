// wuzp-expect: error
// wuzp-error: no matching (member |overloaded )?function

// shift() takes the point to shift to as a coordinate system, a tag, not as a point with coordinates (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> acceleration{
      AngularAccelerationInA{0.0, 0.0, 1.0}, AccelerationInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocity{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                   VelocityInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto shifted =
      acceleration.shift<wuzp::PositionPoint3<ValueMatrix, double, CoordinateSystemA>>(PositionInA{1.0, 0.0, 0.0}, velocity);
  return 0;
}
