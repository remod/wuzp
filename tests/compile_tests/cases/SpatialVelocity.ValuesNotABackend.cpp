// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The values of a spatial velocity come from a value matrix backend, given as its ValueMatrix, not as the matrices of its
// linear algebra library (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::SpatialVelocity<NotAValueMatrixBackend, double, CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA>);
  return 0;
}
