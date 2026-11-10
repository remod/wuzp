// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The values of a passive linear transformation come from a value matrix backend, given as its ValueMatrix, not as the
// matrices of its linear algebra library.

#include "wuzp/trafo/PassiveLinearTransformation.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::PassiveLinearTransformation<NotAValueMatrixBackend, double, CoordinateSystemA, CoordinateSystemB, 3, 3, Dimensionless>);
  return 0;
}
