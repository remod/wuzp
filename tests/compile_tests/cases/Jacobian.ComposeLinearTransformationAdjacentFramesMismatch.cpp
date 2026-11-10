// wuzp-expect: error
// wuzp-static-assert: Adjacent frames do not match\.

// A change of basis composes with a Jacobian by the chain rule, S_CB * J_BA = J_CA, so the coordinate system From of the
// left-hand side must be To of the right-hand side: S_CB * J_DA is a mistake.

#include "wuzp/trafo/Jacobian.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using ScalingCB = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 2, Dimensionless>;
using JacobianDA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemD, 2, 2, Dimensionless>;

int main() {
  const ScalingCB scalingCB{2.0, 0.0, 0.0, 2.0};
  const JacobianDA jacobianDA{0.0, -1.0, 1.0, 0.0};
  [[maybe_unused]] const auto result = scalingCB * jacobianDA;
  return 0;
}
