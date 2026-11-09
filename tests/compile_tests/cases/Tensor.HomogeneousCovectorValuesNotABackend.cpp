// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The values of a tensor alias come from a value matrix backend, given as its ValueMatrix, e.g. wuzp_eigen::ValueMatrix, not
// as the matrices of its linear algebra library, e.g. Eigen::Array.

#include "wuzp/core/Tensor.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::HomogeneousCovector<3, NotAValueMatrixBackend, double, Length, CoordinateSystemA>);
  return 0;
}
