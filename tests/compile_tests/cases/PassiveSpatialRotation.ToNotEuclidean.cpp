// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The orthogonality of a rotation is defined by the Euclidean metric, so a rotation transforms to a Euclidean coordinate system.

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, SpacetimeB>);
  return 0;
}
