// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The values of a Lorentz transformation come from a value matrix backend, given as its ValueMatrix, not as the matrices of its
// linear algebra library.

#include "wuzp/trafo/LorentzTransformation.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::LorentzTransformation4<NotAValueMatrixBackend, double, CoordinateSystemA, CoordinateSystemB>);
  return 0;
}
