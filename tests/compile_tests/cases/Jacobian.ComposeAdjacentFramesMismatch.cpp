// wuzp-expect: error
// wuzp-static-assert: Adjacent frames do not match\.

// Jacobians compose by the chain rule, J_CB * J_BA = J_CA, so the coordinate system From of the left-hand side must be To of the
// right-hand side: J_CB * J_DA is a mistake, although the dimensions match.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using JacobianCB = Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 2, 2, Dimensionless>;
using JacobianDA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemD, 2, 2, Dimensionless>;

int main() {
  const JacobianCB jacobianCB{1.0, 1.0, 1.0, -1.0};
  const JacobianDA jacobianDA{0.0, -1.0, 1.0, 0.0};
  [[maybe_unused]] const auto result = jacobianCB * jacobianDA;
  return 0;
}
