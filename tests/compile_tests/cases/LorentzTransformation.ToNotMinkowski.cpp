// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A Lorentz transformation preserves the Minkowski metric, so it transforms to a coordinate system with the Minkowski metric,
// an inertial frame, not to a Euclidean one.

#include "wuzp/trafo/LorentzTransformation.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::LorentzTransformation4<ValueMatrix, double, SpacetimeA, CoordinateSystemB>);
  return 0;
}
