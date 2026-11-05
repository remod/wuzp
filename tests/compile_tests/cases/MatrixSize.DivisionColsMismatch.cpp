// wuzp-expect: error
// wuzp-static-assert: Number of cols do not match\.

// Only matrices of the same size can be divided elementwise.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::Division<wuzp::MatrixSize<2, 2>, wuzp::MatrixSize<2, 3>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
