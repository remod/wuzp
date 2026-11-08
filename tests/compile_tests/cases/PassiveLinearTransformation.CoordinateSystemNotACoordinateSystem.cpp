// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A passive linear transformation is expressed in a coordinate system, not in the identifier of one.

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemAId, CoordinateSystemA, CoordinateSystemB>);
  return 0;
}
