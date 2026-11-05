// wuzp-expect: error
// wuzp-static-assert: Types are heterogeneous\.

// Only a unit matrix with a single unit has 'the' unit.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::GetType<wuzp::TypeMatrix<1, 2, Length, Time>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
