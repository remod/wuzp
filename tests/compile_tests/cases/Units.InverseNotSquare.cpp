// wuzp-expect: error
// wuzp-static-assert: Matrix is not square\.

// Only square unit matrices can be inverted.

#include "wuzp/core/Units.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::Inverse<wuzp::TypeMatrix<2, 3, Length>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
