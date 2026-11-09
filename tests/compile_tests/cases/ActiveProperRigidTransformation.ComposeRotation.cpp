// wuzp-expect: error
// wuzp-static-assert: These transformations cannot be composed\.

// A proper rigid transformation composes with proper rigid transformations and translations, but not with a rotation: rotations
// are not told apart from other linear transformations yet, so the result could be a transformation that is not rigid, whose
// inverse (R^T, -R^T t) would be wrong. Compose with a proper rigid transformation that only rotates instead.

#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using RigidTransformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;

int main() {
  const RigidTransformation transformation{RigidTransformation::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                                           RigidTransformation::Translation{0.0, 0.0, 1.0}};
  const ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> rotation{1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0};
  [[maybe_unused]] const auto result = transformation * rotation;
  return 0;
}
