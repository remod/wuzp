// wuzp-expect: error
// wuzp-static-assert: Block bottom right corner is out of range\.

// A block must end inside the matrix.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::Block<wuzp::TypeMatrix<2, 2, Length>, 1, 1, 2, 2>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
