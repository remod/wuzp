// wuzp-expect: error
// wuzp-static-assert: A Jacobian pushes forward vectors and tensors with two upper indices\. Use pullback\(\) for covectors and tensors with two lower indices\.

// The indices of a mixed tensor, e.g. a velocity gradient, would have to be mapped in opposite directions: the upper one pushed
// forward from From to To, the lower one pulled back from To to From. Transforming it from From to To, J T J^-1, would need the
// inverse of the Jacobian, which a Jacobian that is not square does not have.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using JacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
using VelocityGradient =
    Tensor<Indices<Up, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Frequency>>, CoordinateSystemA, Relative>;

int main() {
  const JacobianBA jacobian{1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
  const VelocityGradient velocityGradient{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = jacobian.transform(velocityGradient);
  return 0;
}
