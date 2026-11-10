// wuzp-expect: error
// wuzp-static-assert: These transformations cannot be composed\.

// A passive proper rigid transformation composes with proper rigid transformations, rotations and translations, but not with a
// linear transformation that may not be a rotation, e.g. a scaling: the result would not be rigid, and its inverse
// (R^T, -R^T t) would be wrong. The adjacent frames match, so only the composition itself is rejected.

#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using RigidTransformationBA = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
using ScalingCB = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 3, 3, Dimensionless>;

int main() {
  const RigidTransformationBA rigidBA{RigidTransformationBA::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                                      RigidTransformationBA::Translation{0.0, 0.0, 1.0}};
  const ScalingCB scalingCB{2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0};
  [[maybe_unused]] const auto result = scalingCB * rigidBA;
  return 0;
}
