// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The point a spatial acceleration is about is the origin of a Euclidean coordinate system (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::SpatialAcceleration<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, SpacetimeA, CoordinateSystemA>);
  return 0;
}
