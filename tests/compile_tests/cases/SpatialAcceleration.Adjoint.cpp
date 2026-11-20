// wuzp-expect: error
// wuzp-static-assert: The adjoint map of a rigid transformation maps spatial velocities and spatial forces\. Shifting a spatial acceleration needs the angular velocity: re-express it with transform\(\) and shift\(\) it\.

// The spatial accelerations of Drake, the accelerations of points, shift with the angular velocity of their frame, which the
// adjoint map does not take (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const wuzp::PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double> transformation{};
  const SpatialAccelerationD<CoordinateSystemC, CoordinateSystemD, CoordinateSystemA, CoordinateSystemA> acceleration{
      AngularAccelerationInA{0.0, 0.0, 1.0}, AccelerationInA{1.0, 0.0, 0.0}};
  wuzp::adjoint(transformation, acceleration);
  return 0;
}
