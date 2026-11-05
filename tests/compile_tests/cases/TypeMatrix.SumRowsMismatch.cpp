// wuzp-expect: error
// wuzp-static-assert: Matrix sizes do not match\.

// Only unit matrices of the same size can be summed.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::Sum<wuzp::TypeMatrix<2, 2, Length>, wuzp::TypeMatrix<3, 2, Length>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
