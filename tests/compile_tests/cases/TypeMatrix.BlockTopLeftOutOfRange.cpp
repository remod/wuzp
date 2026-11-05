// wuzp-expect: error
// wuzp-static-assert: Block top left corner is out of range\.

// A block must start inside the matrix.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::Block<wuzp::TypeMatrix<2, 2, Length>, -1, 0, 1, 1>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
