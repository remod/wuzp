// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A Lorentz transformation preserves the Minkowski metric, so it transforms from a coordinate system with the Minkowski metric,
// an inertial frame, not from a Euclidean one.

#include "wuzp/trafo/LorentzTransformation.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::LorentzTransformation4<ValueMatrix, double, CoordinateSystemA, SpacetimeB>);
  return 0;
}
