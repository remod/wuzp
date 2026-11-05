// wuzp-expect: error
// wuzp-static-assert: Cross product is only defined for three-dimensional vectors\.

// The cross product is only defined for three-dimensional vectors.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::CrossProduct<wuzp::MatrixSize<2, 1>, wuzp::MatrixSize<2, 1>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
