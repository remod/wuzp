// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The orthogonality of a rotation is defined by the Euclidean metric, so a rotation is expressed in a Euclidean coordinate
// system. In a Minkowski coordinate system, a spatial rotation is a Lorentz transformation.

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::ActiveSpatialRotation3<ValueMatrix, double, SpacetimeA>);
  return 0;
}
