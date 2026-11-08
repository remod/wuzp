// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A passive translation transforms from a coordinate system, not from the identifier of one.

#include "wuzp/trafo/PassiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemAId, CoordinateSystemB>);
  return 0;
}
