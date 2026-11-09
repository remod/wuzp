// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The values of a passive translation come from a value matrix backend, given as its ValueMatrix, not as the matrices of its
// linear algebra library.

#include "wuzp/trafo/PassiveTranslation.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::PassiveSpatialTranslation3<NotAValueMatrixBackend, double, CoordinateSystemA, CoordinateSystemB>);
  return 0;
}
