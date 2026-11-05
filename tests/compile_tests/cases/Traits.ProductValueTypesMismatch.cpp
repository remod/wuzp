// wuzp-expect: error
// wuzp-static-assert: Value types mismatch\.

// The elementwise 'Product' of values of different numeric types is rejected.

#include "wuzp/core/traits.hpp"

int main() {
  using Type = wuzp::Product<wuzp::ValueType<int>, wuzp::ValueType<double>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
