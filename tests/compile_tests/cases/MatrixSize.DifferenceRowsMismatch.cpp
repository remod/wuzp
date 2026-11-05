// wuzp-expect: error
// wuzp-static-assert: Number of rows do not match\.

// Only matrices of the same size can be subtracted.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::Difference<wuzp::MatrixSize<2, 2>, wuzp::MatrixSize<3, 2>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
