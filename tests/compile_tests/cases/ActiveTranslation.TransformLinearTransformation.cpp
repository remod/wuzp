// wuzp-expect: error
// wuzp-static-assert: This transformation does not transform this transformation\. Compose transformations with operator\*\.

// The conjugation T Q T^-1 of a linear transformation Q by a translation T maps x to Q x + (I - Q) t, which is affine, not linear:
// it returned Q unchanged, which is wrong for points (#72). Compose with operator* and inverse() instead.

#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> translation{0.0, 0.0, 1.0};
  const ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> rotation{1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0};
  [[maybe_unused]] const auto result = translation.transform(rotation);
  return 0;
}
