// wuzp-expect: error
// wuzp-static-assert: Matrix is not square\.

// Only square matrices can be inverted.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::Inverse<wuzp::MatrixSize<2, 3>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
