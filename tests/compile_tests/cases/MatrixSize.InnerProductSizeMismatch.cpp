// wuzp-expect: error
// wuzp-static-assert: Number of LHS cols does not match number of RHS rows\.

// A matrix product requires the columns of the left matrix to match the rows of the right matrix.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::InnerProduct<wuzp::MatrixSize<2, 3>, wuzp::MatrixSize<2, 3>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
