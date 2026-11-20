// wuzp-expect: error
// wuzp-static-assert: Frames do not match\.

// The spatial velocities of two different frames, e.g. two bodies, cannot be subtracted (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocityOfB{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                      VelocityInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemC, PointP, CoordinateSystemA> velocityOfC{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                      VelocityInA{1.0, 1.0, 0.0}};
  [[maybe_unused]] const auto difference = velocityOfB - velocityOfC;
  return 0;
}
