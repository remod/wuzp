// wuzp-expect: error
// wuzp-static-assert: Hypermatrices are not supported\. At least one of the matrices must be a scalar or a vector\.

// The outer product of two column matrices with several rows would be a hypermatrix.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::OuterProduct<wuzp::MatrixSize<2, 1>, wuzp::MatrixSize<2, 1>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
