// wuzp-expect: error
// wuzp-static-assert: Matrix sizes do not match\.

// The dot product requires vectors of the same size.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::DotProduct<wuzp::MatrixSize<2, 1>, wuzp::MatrixSize<3, 1>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
