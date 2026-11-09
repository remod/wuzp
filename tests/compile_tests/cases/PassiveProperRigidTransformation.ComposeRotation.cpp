// wuzp-expect: error
// wuzp-static-assert: These transformations cannot be composed\.

// A passive proper rigid transformation composes with proper rigid transformations and translations, but not with a rotation:
// rotations are not told apart from other linear transformations yet, so the result could be a transformation that is not rigid,
// whose inverse (R^T, -R^T t) would be wrong. The adjacent frames match, so only the composition itself is rejected.

#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using RigidTransformationBA = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;

int main() {
  const RigidTransformationBA rigidBA{RigidTransformationBA::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                                      RigidTransformationBA::Translation{0.0, 0.0, 1.0}};
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC> rotationCB{1.0, 0.0, 0.0, 0.0, 1.0,
                                                                                                      0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = rotationCB * rigidBA;
  return 0;
}
