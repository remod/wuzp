// wuzp-expect: error
// wuzp-static-assert: The vector must be expressed in From or To of the passive rotation\.

// The increment of boxPlus of a passive rotation may be expressed in From or in To, but not in another coordinate system (#77).

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> rotation;
  const wuzp::RotationVector3<ValueMatrix, double, CoordinateSystemC> increment{0.1, 0.2, 0.3};
  [[maybe_unused]] const auto perturbed = rotation.boxPlus(increment);
  return 0;
}
