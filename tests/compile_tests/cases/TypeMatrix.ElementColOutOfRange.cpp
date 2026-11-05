// wuzp-expect: error
// wuzp-static-assert: Element columns id is out of range\.

// Elements outside the matrix cannot be accessed.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::Element<wuzp::TypeMatrix<2, 2, Length>, 0, 2>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
