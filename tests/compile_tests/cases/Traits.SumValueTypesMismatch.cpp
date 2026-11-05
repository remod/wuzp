// wuzp-expect: error
// wuzp-static-assert: Value types mismatch\.

// The elementwise 'Sum' of values of different numeric types is rejected.

#include "wuzp/core/traits.hpp"

int main() {
  using Type = wuzp::Sum<wuzp::ValueType<int>, wuzp::ValueType<double>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
