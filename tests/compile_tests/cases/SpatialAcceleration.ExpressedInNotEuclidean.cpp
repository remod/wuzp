// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The parts of a spatial acceleration are three-dimensional vectors of a cross product, so they are expressed in a Euclidean
// coordinate system (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::SpatialAcceleration<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, PointP, SpacetimeA>);
  return 0;
}
