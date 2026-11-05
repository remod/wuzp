// wuzp-expect: error
// wuzp-static-assert: Block bottom right corner is out of range\.

// A block must end inside the matrix.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::Block<wuzp::MatrixSize<2, 2>, 1, 1, 2, 2>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
