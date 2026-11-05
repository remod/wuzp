// wuzp-expect: error
// wuzp-static-assert: Number of cols do not match\.

// Only matrices of the same size can be combined in a tensor product.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::TensorProduct<wuzp::MatrixSize<2, 2>, wuzp::MatrixSize<2, 3>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
