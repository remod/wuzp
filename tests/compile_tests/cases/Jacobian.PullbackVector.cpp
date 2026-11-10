// wuzp-expect: error
// wuzp-static-assert: The pullback by a Jacobian maps covectors and tensors with two lower indices\. Use transform\(\) for vectors and tensors with two upper indices\.

// A vector, e.g. a velocity, is pushed forward from From to To with transform(): pulling it back from To to From would need the
// inverse of the Jacobian.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using JacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 2, 2, Dimensionless>;

int main() {
  const JacobianBA jacobian{0.0, -1.0, 1.0, 0.0};
  const VelocityVector2<ValueMatrix, double, CoordinateSystemB> velocity{1.0, 2.0};
  [[maybe_unused]] const auto result = jacobian.pullback(velocity);
  return 0;
}
