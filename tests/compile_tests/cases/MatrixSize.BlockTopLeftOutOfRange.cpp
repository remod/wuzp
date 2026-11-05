// wuzp-expect: error
// wuzp-static-assert: Block top left corner is out of range\.

// A block must start inside the matrix.

#include "wuzp/core/MatrixSize.hpp"

int main() {
  using Type = wuzp::Block<wuzp::MatrixSize<2, 2>, -1, 0, 1, 1>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
