// wuzp-expect: error
// wuzp-static-assert: The object is already relative\.

// Making a relative object relative again is a mistake.

#include "wuzp/core/Absolute.hpp"

int main() {
  using Type = wuzp::MakeRelative<wuzp::Relative>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
