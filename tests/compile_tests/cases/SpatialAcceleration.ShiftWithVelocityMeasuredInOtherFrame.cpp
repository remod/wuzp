// wuzp-expect: error
// wuzp-static-assert: Measured-in frames do not match\.

// Shifting the spatial acceleration of B measured in A needs the angular velocity of B measured in A, not in another frame (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> acceleration{
      AngularAccelerationInA{0.0, 0.0, 1.0}, AccelerationInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemC, CoordinateSystemB, PointP, CoordinateSystemA> velocityInC{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                      VelocityInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto shifted = acceleration.shift<PointQ>(PositionInA{1.0, 0.0, 0.0}, velocityInC);
  return 0;
}
