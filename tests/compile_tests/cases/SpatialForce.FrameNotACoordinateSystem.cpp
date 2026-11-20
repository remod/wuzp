// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The frame a spatial force acts on is a coordinate system (#76).

#include "wuzp/trafo/SpatialForce.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::SpatialForce<ValueMatrix, double, int, PointP, CoordinateSystemA>);
  return 0;
}
