// wuzp-expect: error
// wuzp-static-assert: These transformations cannot be composed\.

// Jacobians compose with Jacobians and changes of basis by the chain rule, but not with translations, which move points and whose
// composition with a linear map is affine. The adjacent frames match, so only the composition itself is rejected.

#include "wuzp/trafo/Jacobian.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using JacobianCB = Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 2, 2, Dimensionless>;
using TranslationBA = PassiveSpatialTranslation2<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;

int main() {
  const JacobianCB jacobianCB{0.0, -1.0, 1.0, 0.0};
  const TranslationBA translationBA{1.0, 2.0};
  [[maybe_unused]] const auto result = jacobianCB * translationBA;
  return 0;
}
