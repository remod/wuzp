// wuzp-expect: error
// wuzp-static-assert: 'To' coordinate systems do not match\.

// Passive transformations can only be compared approximately if they transform between the same coordinate systems, also
// Jacobians, whose matrices are not square (#68).

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Lhs = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
using Rhs = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC, 3, 2, Dimensionless>;

int main() {
  const Lhs lhs{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  const Rhs rhs{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  return lhs.isApprox(rhs) ? 0 : 1;
}
