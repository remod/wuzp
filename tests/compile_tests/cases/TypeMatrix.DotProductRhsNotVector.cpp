// wuzp-expect: error
// wuzp-static-assert: Dot product is only defined for vectors\.

// The dot product is only defined for vectors.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::DotProduct<wuzp::TypeMatrix<3, 1, Length>, wuzp::TypeMatrix<3, 2, Length>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
