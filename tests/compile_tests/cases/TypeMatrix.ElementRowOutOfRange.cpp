// wuzp-expect: error
// wuzp-static-assert: Element row id is out of range\.

// Elements outside the matrix cannot be accessed.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::Element<wuzp::TypeMatrix<2, 2, Length>, 2, 0>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
