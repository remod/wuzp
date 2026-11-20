// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// The spatial velocity and the spatial acceleration of the moving frame are expressed in the coordinate system of this spatial
// acceleration: re-express them there first (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> accelerationAB{
      AngularAccelerationInA{0.0, 0.0, 1.0}, AccelerationInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocityAB{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                     VelocityInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemB> velocityBCInB{AngularVelocityInB{1.0, 0.0, 0.0},
                                                                                                        VelocityInB{0.0, 0.0, 1.0}};
  const SpatialAccelerationD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemB> accelerationBCInB{
      AngularAccelerationInB{1.0, 0.0, 0.0}, AccelerationInB{0.0, 0.0, 1.0}};
  [[maybe_unused]] const auto composed =
      accelerationAB.composeWithMovingFrameAcceleration(PositionInA{0.0, 1.0, 0.0}, velocityAB, velocityBCInB, accelerationBCInB);
  return 0;
}
