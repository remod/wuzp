// wuzp-expect: error
// wuzp-static-assert: Reference points do not match\.

// The spatial velocities of a body about two different points cannot be added: shift() one of them to the point of the other
// (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocityOfP{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                      VelocityInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointQ, CoordinateSystemA> velocityOfQ{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                      VelocityInA{1.0, 1.0, 0.0}};
  [[maybe_unused]] const auto sum = velocityOfP + velocityOfQ;
  return 0;
}
