// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The point a spatial velocity is about is the origin of a coordinate system, a tag, not a point with coordinates (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "compile_test_types.hpp"

int main() {
  using Point = wuzp::PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::SpatialVelocity<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, Point, CoordinateSystemA>);
  return 0;
}
