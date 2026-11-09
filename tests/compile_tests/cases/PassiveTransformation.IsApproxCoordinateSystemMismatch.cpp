// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Only transformations whose matrices or vectors are expressed in the same coordinate system can be compared approximately.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Lhs = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
using Rhs = Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;

int main() {
  Lhs lhs{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  const Rhs rhs{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  return lhs.isApprox(rhs) ? 0 : 1;
}
