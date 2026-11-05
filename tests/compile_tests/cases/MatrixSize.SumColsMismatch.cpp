// wuzp-expect: error
// wuzp-static-assert: Number of cols do not match\.

// Only matrices of the same size can be summed.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::Sum<wuzp::MatrixSize<2, 2>, wuzp::MatrixSize<2, 3>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
