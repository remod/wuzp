// wuzp-expect: error
// wuzp-static-assert: A Jacobian pushes forward vectors and tensors with two upper indices\. Use pullback\(\) for covectors and tensors with two lower indices\.

// A tensor with two lower indices, e.g. a mass matrix, is pulled back from To to From, J^T M J, e.g. into the joint space inertia.
// Transforming it from From to To would need the inverse of the Jacobian, which a Jacobian that is not square does not have.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using JacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
using MassMatrix = Tensor<Indices<Down, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Mass>>, CoordinateSystemA, Relative>;

int main() {
  const JacobianBA jacobian{1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
  const MassMatrix massMatrix{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = jacobian.transform(massMatrix);
  return 0;
}
