// wuzp-expect: error
// wuzp-static-assert: Hypermatrices are not supported\. At least one of the matrices must be a scalar or a vector\.

// The outer product of two row matrices with several columns would be a hypermatrix.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::OuterProduct<wuzp::TypeMatrix<1, 2, Length>, wuzp::TypeMatrix<1, 2, Length>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
