// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A passive linear transformation transforms from a coordinate system, not from the identifier of one.

#include "wuzp/trafo/PassiveLinearTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemAId, CoordinateSystemB, 3, Dimensionless>);
  return 0;
}
