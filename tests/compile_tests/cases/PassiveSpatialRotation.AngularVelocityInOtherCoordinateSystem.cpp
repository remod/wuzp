// wuzp-expect: error
// wuzp-static-assert: The vector must be expressed in From or To of the passive rotation\.

// The angular velocity of From relative to To is expressed in From or in To, but not in another coordinate system (#77).

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  using Rotation = wuzp::PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  const Rotation::EulerAnglesT angles{0.1, 0.2, 0.3};
  const Rotation::EulerAnglesDerivativeT derivative{0.1, 0.2, 0.3};
  [[maybe_unused]] const auto angularVelocity =
      Rotation::angularVelocityFromEulerAnglesZyxDerivative<CoordinateSystemC>(angles, derivative);
  return 0;
}
