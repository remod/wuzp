// wuzp-expect: error
// wuzp-static-assert: These transformations cannot be composed\.

// A passive translation and a passive linear transformation do not compose: the result would be affine, and rotations are not
// told apart from other linear transformations yet. The adjacent frames match, so only the composition itself is rejected.

#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> rotationBA{1.0, 0.0, 0.0, 0.0, 1.0,
                                                                                                      0.0, 0.0, 0.0, 1.0};
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC> translationCB{0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = translationCB * rotationBA;
  return 0;
}
