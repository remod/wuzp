// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Active translations can only be compared approximately if they are expressed in the same coordinate system (#68).

#include "wuzp/trafo/ActiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> lhs{0.0, 0.0, 1.0};
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemB> rhs{0.0, 0.0, 1.0};
  return lhs.isApprox(rhs, 1e-12) ? 0 : 1;
}
