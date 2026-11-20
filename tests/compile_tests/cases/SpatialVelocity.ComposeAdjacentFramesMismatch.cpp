// wuzp-expect: error
// wuzp-static-assert: Adjacent frames do not match\.

// V_MB composes with V_BC into V_MC, like X_AB * X_BC: the velocity of the moving frame must be measured in the frame of this
// spatial velocity (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocityAB{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                     VelocityInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, PointQ, CoordinateSystemA> velocityDC{AngularVelocityInA{1.0, 0.0, 0.0},
                                                                                                     VelocityInA{0.0, 0.0, 1.0}};
  [[maybe_unused]] const auto composed = velocityAB.composeWithMovingFrameVelocity(PositionInA{0.0, 1.0, 0.0}, velocityDC);
  return 0;
}
