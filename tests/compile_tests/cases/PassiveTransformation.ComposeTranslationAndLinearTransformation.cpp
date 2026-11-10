// wuzp-expect: error
// wuzp-static-assert: These transformations cannot be composed\.

// A passive translation composes with rotations into proper rigid transformations, but not with a linear transformation that
// may not be a rotation, e.g. a scaling: the result would be affine, but not rigid. The adjacent frames match, so only the
// composition itself is rejected.

#include "wuzp/trafo/PassiveLinearTransformation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, Dimensionless> scalingBA{
      2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0};
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC> translationCB{0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = translationCB * scalingBA;
  return 0;
}
