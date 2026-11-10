// wuzp-expect: error
// wuzp-static-assert: These transformations cannot be composed\.

// A translation composes with rotations into proper rigid transformations, but not with a linear transformation that may not be
// a rotation, e.g. a scaling: the result would be affine, but not rigid.

#include "wuzp/trafo/ActiveLinearTransformation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> translation{0.0, 0.0, 1.0};
  const ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 3, Dimensionless> scaling{2.0, 0.0, 0.0, 0.0, 2.0,
                                                                                                        0.0, 0.0, 0.0, 2.0};
  [[maybe_unused]] const auto result = translation * scaling;
  return 0;
}
