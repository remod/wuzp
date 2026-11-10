// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A Jacobian maps into a coordinate system, not into the identifier of one.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemBId, 3, 2, Dimensionless>);
  return 0;
}
