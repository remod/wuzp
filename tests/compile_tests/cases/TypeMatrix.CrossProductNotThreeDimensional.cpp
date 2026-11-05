// wuzp-expect: error
// wuzp-static-assert: Cross product is only defined for three-dimensional vectors\.

// The cross product is only defined for three-dimensional vectors.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::CrossProduct<wuzp::TypeMatrix<2, 1, Length>, wuzp::TypeMatrix<2, 1, Length>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
