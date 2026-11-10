// wuzp-expect: error
// wuzp-static-assert: A translation only transforms points, vectors, covectors and tensors with two upper indices: other tensors may depend on the origin, e\.g\. an inertia about it\.

// A rigid transformation rotates a tensor with two lower indices, but its translation rejects it: it may be an inertia about the
// origin, which changes with the origin by the parallel axis theorem.

#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using RigidTransformationBA = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
using Inertia = Tensor<Indices<Down, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Product<Mass, Area>::type>>,
                       CoordinateSystemA, Relative>;

int main() {
  const RigidTransformationBA rigidBA{RigidTransformationBA::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                                      RigidTransformationBA::Translation{0.0, 0.0, 1.0}};
  const Inertia inertia{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = rigidBA.transform(inertia);
  return 0;
}
