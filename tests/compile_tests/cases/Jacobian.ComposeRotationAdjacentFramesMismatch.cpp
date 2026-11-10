// wuzp-expect: error
// wuzp-static-assert: Adjacent frames do not match\.

// A Jacobian composes with a rotation by the chain rule, J_CB * R_BD = J_CD, so the coordinate system From of the left-hand side
// must be To of the right-hand side: J_CB * R_AD is a mistake.

#include "wuzp/trafo/Jacobian.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using JacobianCB = Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 2, 2, Dimensionless>;
using RotationAD = PassiveSpatialRotation2<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA>;

int main() {
  const JacobianCB jacobianCB{1.0, 1.0, 1.0, -1.0};
  const RotationAD rotationAD{0.0, 1.0, -1.0, 0.0};
  [[maybe_unused]] const auto result = jacobianCB * rotationAD;
  return 0;
}
