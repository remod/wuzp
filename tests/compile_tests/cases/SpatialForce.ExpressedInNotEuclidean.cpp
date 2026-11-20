// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The parts of a spatial force are three-dimensional vectors of a cross product, so they are expressed in a Euclidean coordinate
// system (#76).

#include "wuzp/trafo/SpatialForce.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::SpatialForce<ValueMatrix, double, CoordinateSystemB, PointP, SpacetimeA>);
  return 0;
}
