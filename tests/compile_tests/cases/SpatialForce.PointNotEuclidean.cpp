// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The point the torque of a spatial force is about is the origin of a Euclidean coordinate system (#76).

#include "wuzp/trafo/SpatialForce.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::SpatialForce<ValueMatrix, double, CoordinateSystemB, SpacetimeA, CoordinateSystemA>);
  return 0;
}
