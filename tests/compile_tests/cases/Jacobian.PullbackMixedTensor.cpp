// wuzp-expect: error
// wuzp-static-assert: The pullback by a Jacobian maps covectors and tensors with two lower indices\. Use transform\(\) for vectors and tensors with two upper indices\.

// The indices of a mixed tensor, e.g. a velocity gradient, would have to be mapped in opposite directions: the lower one pulled
// back from To to From, the upper one pushed forward from From to To. Pulling it back from To to From, J^-1 T J, would need the
// inverse of the Jacobian, which a Jacobian that is not square does not have.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using JacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
using VelocityGradient =
    Tensor<Indices<Up, Down>, Quantities<ValueMatrix<double, 2, 2>, TypeMatrix<2, 2, Frequency>>, CoordinateSystemB, Relative>;

int main() {
  const JacobianBA jacobian{1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
  const VelocityGradient velocityGradient{1.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = jacobian.pullback(velocityGradient);
  return 0;
}
