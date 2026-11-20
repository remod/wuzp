// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The values of a spatial acceleration come from a value matrix backend, given as its ValueMatrix, not as the matrices of its
// linear algebra library (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::SpatialAcceleration<NotAValueMatrixBackend, double, CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA>);
  return 0;
}
