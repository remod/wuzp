// wuzp-expect: error
// wuzp-static-assert: Reference points do not match\.

// The spatial velocity and the spatial acceleration of the moving frame are about the same point, to which the composition shifts
// (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> accelerationAB{
      AngularAccelerationInA{0.0, 0.0, 1.0}, AccelerationInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocityAB{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                     VelocityInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemB, CoordinateSystemC, PointP, CoordinateSystemA> velocityBC{AngularVelocityInA{1.0, 0.0, 0.0},
                                                                                                     VelocityInA{0.0, 0.0, 1.0}};
  const SpatialAccelerationD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemA> accelerationBC{
      AngularAccelerationInA{1.0, 0.0, 0.0}, AccelerationInA{0.0, 0.0, 1.0}};
  [[maybe_unused]] const auto composed =
      accelerationAB.composeWithMovingFrameAcceleration(PositionInA{0.0, 1.0, 0.0}, velocityAB, velocityBC, accelerationBC);
  return 0;
}
