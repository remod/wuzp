// wuzp-expect: error
// wuzp-static-assert: The object is already absolute\.

// Making an absolute object absolute again is a mistake.

#include "wuzp/core/Absolute.hpp"

int main() {
  using Type = wuzp::MakeAbsolute<wuzp::Absolute>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
