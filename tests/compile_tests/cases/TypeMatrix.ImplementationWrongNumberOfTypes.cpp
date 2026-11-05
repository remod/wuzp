// wuzp-expect: error
// wuzp-static-assert: Number of types does not match matrix size\.

// A unit matrix needs exactly one unit per element.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::TypeMatrixImpl<2, 2, std::tuple<Length>>;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
