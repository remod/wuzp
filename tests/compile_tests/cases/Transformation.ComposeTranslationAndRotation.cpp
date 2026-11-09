// wuzp-expect: error
// wuzp-static-assert: These transformations cannot be composed\.

// A translation and a linear transformation do not compose: the result would be affine, and rotations are not told apart from
// other linear transformations yet. A proper rigid transformation composes with translations.

#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> translation{0.0, 0.0, 1.0};
  const ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = translation * rotation;
  return 0;
}
