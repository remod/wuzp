// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot be powered\.

// Raising an absolute object (e.g. a point) to a power is not defined.

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/Integer.hpp"

int main() {
  using Type = wuzp::Power<wuzp::Absolute, wuzp::Integer<2>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
