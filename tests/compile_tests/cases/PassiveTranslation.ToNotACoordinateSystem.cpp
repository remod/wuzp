// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A passive translation transforms into a coordinate system, not into the identifier of one.

#include "wuzp/trafo/PassiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemBId>);
  return 0;
}
