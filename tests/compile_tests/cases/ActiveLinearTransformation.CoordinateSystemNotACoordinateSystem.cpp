// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A linear transformation is expressed in a coordinate system, not in the identifier of one.

#include "wuzp/trafo/ActiveLinearTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemAId, 3, 3, Dimensionless>);
  return 0;
}
