// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The frame a spatial acceleration is measured in is a coordinate system (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::SpatialAcceleration<ValueMatrix, double, int, CoordinateSystemB, PointP, CoordinateSystemA>);
  return 0;
}
