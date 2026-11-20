// wuzp-expect: error
// wuzp-static-assert: The adjoint map of the rigid transformation X_AB maps spatial vectors about the origin of B, expressed in B, to the origin of A, expressed in A: shift\(\) them to the origin of B first\.

// The adjoint map of X_AB, from B to A, shifts spatial velocities from the origin of B to the origin of A, so the spatial velocity
// must be about the origin of B (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const wuzp::PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double> transformation{};
  const SpatialVelocityD<CoordinateSystemC, CoordinateSystemD, PointP, CoordinateSystemA> velocity{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                   VelocityInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto mapped = wuzp::adjoint(transformation, velocity);
  return 0;
}
