// wuzp-expect: error
// wuzp-requires: values=eigen
// wuzp-static-assert: This transformation does not transform this transformation\. Compose transformations with operator\*\.

// The conjugation G Q G^-1 of a linear transformation Q by a proper rigid transformation G is affine, not linear: it returned
// R Q R^-1, which is wrong for points (#72). Compose with operator* and inverse() instead.
// Only with Eigen: the rotation conjugates Q first, which needs a matrix inverse, so with Blitz++ some compilers stop at its
// error. ActiveTranslation.TransformLinearTransformation tests the same with every backend. Once rotations are a class of their
// own, whose inverse is R^T, this case can run with Blitz++ too.

#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using RigidTransformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;

int main() {
  const RigidTransformation transformation{RigidTransformation::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                                           RigidTransformation::Translation{0.0, 0.0, 1.0}};
  const RigidTransformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0};
  [[maybe_unused]] const auto result = transformation.transform(rotation);
  return 0;
}
