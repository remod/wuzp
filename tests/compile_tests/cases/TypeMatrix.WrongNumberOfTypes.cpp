// wuzp-expect: error
// wuzp-static-assert: Number of types does not match matrix size\.

// A unit matrix is created from one unit (for all elements) or from one unit per element.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::TypeMatrix<2, 2, Length, Length>;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
