// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// A passive rotation from A re-expresses spatial velocities expressed in A, not in another coordinate system (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const wuzp::PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> rotation{};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemC> velocityInC{
      wuzp::AngularVelocityVector3<ValueMatrix, double, CoordinateSystemC>{0.0, 0.0, 1.0},
      wuzp::VelocityVector3<ValueMatrix, double, CoordinateSystemC>{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto transformed = rotation.transform(velocityInC);
  return 0;
}
