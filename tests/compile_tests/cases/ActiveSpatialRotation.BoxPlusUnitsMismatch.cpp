// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// The increment of boxPlus is a rotation vector, whose unit is an angle, not a position (#77).

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> rotation;
  const wuzp::PositionVector3<ValueMatrix, double, CoordinateSystemA> position{0.1, 0.2, 0.3};
  [[maybe_unused]] const auto perturbed = rotation.boxPlus(position);
  return 0;
}
