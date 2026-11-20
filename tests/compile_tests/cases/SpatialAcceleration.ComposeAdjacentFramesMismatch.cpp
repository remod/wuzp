// wuzp-expect: error
// wuzp-static-assert: Adjacent frames do not match\.

// A_MB composes with V_BC and A_BC into A_MC: the motion of the moving frame must be measured in the frame of this spatial
// acceleration (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> accelerationAB{
      AngularAccelerationInA{0.0, 0.0, 1.0}, AccelerationInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocityAB{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                     VelocityInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, PointQ, CoordinateSystemA> velocityDC{AngularVelocityInA{1.0, 0.0, 0.0},
                                                                                                     VelocityInA{0.0, 0.0, 1.0}};
  const SpatialAccelerationD<CoordinateSystemD, CoordinateSystemC, PointQ, CoordinateSystemA> accelerationDC{
      AngularAccelerationInA{1.0, 0.0, 0.0}, AccelerationInA{0.0, 0.0, 1.0}};
  [[maybe_unused]] const auto composed =
      accelerationAB.composeWithMovingFrameAcceleration(PositionInA{0.0, 1.0, 0.0}, velocityAB, velocityDC, accelerationDC);
  return 0;
}
