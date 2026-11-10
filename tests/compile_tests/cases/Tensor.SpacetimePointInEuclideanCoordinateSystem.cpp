// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// An event, a point in spacetime, is expressed in a coordinate system with the Minkowski metric, not in a Euclidean one.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::SpacetimePoint4<ValueMatrix, double, CoordinateSystemA>);
  return 0;
}
