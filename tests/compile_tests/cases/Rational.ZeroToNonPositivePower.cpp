// wuzp-expect: error
// wuzp-static-assert: Zero raised to a non-positive power is not defined\.

// 0^0 and 0 raised to a negative power have no value.

#include "wuzp/core/Rational.hpp"

int main() {
  using Type = wuzp::Power<wuzp::Rational<0, 1>, wuzp::Rational<-1, 2>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
