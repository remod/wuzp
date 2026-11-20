// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// The increment of boxPlus of an active rotation is expressed in its coordinate system (#77).

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> rotation;
  const wuzp::RotationVector3<ValueMatrix, double, CoordinateSystemB> increment{0.1, 0.2, 0.3};
  [[maybe_unused]] const auto perturbed = rotation.boxPlus(increment);
  return 0;
}
