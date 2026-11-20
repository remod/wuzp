// wuzp-expect: error
// wuzp-static-assert: Frames do not match\.

// Shifting the spatial acceleration of B needs the angular velocity of B, not that of another frame (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> acceleration{
      AngularAccelerationInA{0.0, 0.0, 1.0}, AccelerationInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemC, PointP, CoordinateSystemA> velocityOfC{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                      VelocityInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto shifted = acceleration.shift<PointQ>(PositionInA{1.0, 0.0, 0.0}, velocityOfC);
  return 0;
}
