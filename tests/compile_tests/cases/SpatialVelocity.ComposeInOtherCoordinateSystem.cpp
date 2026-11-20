// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// The spatial velocity of the moving frame is expressed in the coordinate system of this spatial velocity: re-express it there first
// (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocityAB{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                     VelocityInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemB> velocityBCInB{AngularVelocityInB{1.0, 0.0, 0.0},
                                                                                                        VelocityInB{0.0, 0.0, 1.0}};
  [[maybe_unused]] const auto composed = velocityAB.composeWithMovingFrameVelocity(PositionInA{0.0, 1.0, 0.0}, velocityBCInB);
  return 0;
}
