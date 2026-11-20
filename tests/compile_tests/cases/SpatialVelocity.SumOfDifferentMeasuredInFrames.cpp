// wuzp-expect: error
// wuzp-static-assert: Measured-in frames do not match\.

// Velocities measured in different frames cannot be added: V_MB + V_BC is not V_MC, which composeWithMovingFrameVelocity() computes
// (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocityAB{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                     VelocityInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemC, CoordinateSystemB, PointP, CoordinateSystemA> velocityCB{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                     VelocityInA{1.0, 1.0, 0.0}};
  [[maybe_unused]] const auto sum = velocityAB + velocityCB;
  return 0;
}
