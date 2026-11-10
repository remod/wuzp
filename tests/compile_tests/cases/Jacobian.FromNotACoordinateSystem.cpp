// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A Jacobian maps from a coordinate system, not from the identifier of one.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(Jacobian<ValueMatrix, double, CoordinateSystemAId, CoordinateSystemB, 3, 2, Dimensionless>);
  return 0;
}
