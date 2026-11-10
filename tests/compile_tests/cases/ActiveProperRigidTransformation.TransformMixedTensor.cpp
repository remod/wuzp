// wuzp-expect: error
// wuzp-static-assert: A translation only transforms points, vectors, covectors and tensors with two upper indices: other tensors may depend on the origin, e\.g\. an inertia about it\.

// A rigid transformation rotates a mixed tensor, R T R^T, but its translation rejects it: it may be the inertia about the origin
// written as the linear map from the angular velocity to the angular momentum, which changes with the origin by the parallel axis
// theorem.

#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using RigidTransformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
using Inertia = Tensor<Indices<Up, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Product<Mass, Area>::type>>,
                       CoordinateSystemA, Relative>;

int main() {
  const RigidTransformation transformation{RigidTransformation::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                                           RigidTransformation::Translation{0.0, 0.0, 1.0}};
  const Inertia inertia{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = transformation.transform(inertia);
  return 0;
}
