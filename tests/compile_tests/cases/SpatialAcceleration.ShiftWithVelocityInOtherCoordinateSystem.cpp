// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Shifting a spatial acceleration needs the angular velocity of its frame expressed in the same coordinate system (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> acceleration{
      AngularAccelerationInA{0.0, 0.0, 1.0}, AccelerationInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemB> velocityInB{AngularVelocityInB{0.0, 0.0, 1.0},
                                                                                                      VelocityInB{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto shifted = acceleration.shift<PointQ>(PositionInA{1.0, 0.0, 0.0}, velocityInB);
  return 0;
}
