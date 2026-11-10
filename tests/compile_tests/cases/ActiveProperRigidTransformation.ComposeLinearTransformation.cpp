// wuzp-expect: error
// wuzp-static-assert: These transformations cannot be composed\.

// A proper rigid transformation composes with proper rigid transformations, rotations and translations, but not with a linear
// transformation that may not be a rotation, e.g. a scaling: the result would not be rigid, and its inverse (R^T, -R^T t) would
// be wrong.

#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"
#include "wuzp/trafo/ActiveLinearTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using RigidTransformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
using Scaling = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 3, Dimensionless>;

int main() {
  const RigidTransformation transformation{RigidTransformation::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                                           RigidTransformation::Translation{0.0, 0.0, 1.0}};
  const Scaling scaling{2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0};
  [[maybe_unused]] const auto result = transformation * scaling;
  return 0;
}
