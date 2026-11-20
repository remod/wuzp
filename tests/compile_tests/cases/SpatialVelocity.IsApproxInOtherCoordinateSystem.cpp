// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Spatial velocities are only compared if they are expressed in the same coordinate system (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocityInA{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                      VelocityInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemC> velocityInC{
      wuzp::AngularVelocityVector3<ValueMatrix, double, CoordinateSystemC>{0.0, 0.0, 1.0},
      wuzp::VelocityVector3<ValueMatrix, double, CoordinateSystemC>{1.0, 0.0, 0.0}};
  [[maybe_unused]] const bool approx = velocityInA.isApprox(velocityInC);
  return 0;
}
