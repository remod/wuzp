// wuzp-expect: error
// wuzp-static-assert: Dot product is only defined for vectors\.

// The dot product is only defined for vectors.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::DotProduct<wuzp::MatrixSize<3, 1>, wuzp::MatrixSize<3, 2>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
