// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// The adjoint map of X_AB, from B to A, re-expresses spatial velocities expressed in B, not in another coordinate system (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const wuzp::PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemB, CoordinateSystemC, double> transformation{};
  const SpatialVelocityD<CoordinateSystemC, CoordinateSystemD, CoordinateSystemB, CoordinateSystemA> velocity{
      AngularVelocityInA{0.0, 0.0, 1.0}, VelocityInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto mapped = wuzp::adjoint(transformation, velocity);
  return 0;
}
