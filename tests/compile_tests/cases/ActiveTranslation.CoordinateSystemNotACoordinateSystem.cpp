// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A translation is expressed in a coordinate system, not in the identifier of one.

#include "wuzp/trafo/ActiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemAId>);
  return 0;
}
