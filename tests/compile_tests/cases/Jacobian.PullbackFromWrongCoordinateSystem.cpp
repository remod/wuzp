// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// The pullback by the Jacobian from A to B maps covectors in B into A, so the covector must be expressed in B.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using JacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 2, 2, Dimensionless>;

int main() {
  const JacobianBA jacobian{0.0, -1.0, 1.0, 0.0};
  const HomogeneousCovector<2, ValueMatrix, double, Force, CoordinateSystemA> force{1.0, 2.0};
  [[maybe_unused]] const auto result = jacobian.pullback(force);
  return 0;
}
