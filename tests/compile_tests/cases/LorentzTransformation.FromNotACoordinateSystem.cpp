// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A Lorentz transformation transforms from an inertial frame, a coordinate system, not from the identifier of one.

#include "wuzp/trafo/LorentzTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(LorentzTransformation4<ValueMatrix, double, CoordinateSystemAId, CoordinateSystemB>);
  return 0;
}
