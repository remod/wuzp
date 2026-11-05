// wuzp-expect: error
// wuzp-static-assert: 'Power' trait not specialized for given type\.

// Powers that are not exact rationals, e.g. 4^(1/4) = sqrt(2), are not defined.

#include "wuzp/core/Rational.hpp"

int main() {
  using Type = wuzp::Power<wuzp::Rational<4, 1>, wuzp::Rational<1, 4>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
