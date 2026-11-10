// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A passive linear transformation transforms to a coordinate system, not to the identifier of one.

#include "wuzp/trafo/PassiveLinearTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, CoordinateSystemBId, 3, 3, Dimensionless>);
  return 0;
}
