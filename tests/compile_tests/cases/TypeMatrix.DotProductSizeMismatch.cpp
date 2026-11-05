// wuzp-expect: error
// wuzp-static-assert: Matrix sizes do not match\.

// The dot product requires vectors of the same size.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::DotProduct<wuzp::TypeMatrix<2, 1, Length>, wuzp::TypeMatrix<3, 1, Length>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
