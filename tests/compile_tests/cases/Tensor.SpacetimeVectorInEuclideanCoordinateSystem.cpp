// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A spacetime vector is expressed in a coordinate system with the Minkowski metric, in which the Euclidean dot product and norm
// are rejected, not in a Euclidean one.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::SpacetimeVector4<ValueMatrix, double, CoordinateSystemA>);
  return 0;
}
