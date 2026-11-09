// wuzp-expect: error
// wuzp-static-assert: 'To' coordinate systems do not match\.

// Passive translations can only be compared if they translate between the same coordinate systems.

#include "wuzp/trafo/PassiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> lhs{0.0, 0.0, 1.0};
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC> rhs{0.0, 0.0, 1.0};
  return lhs == rhs ? 0 : 1;
}
