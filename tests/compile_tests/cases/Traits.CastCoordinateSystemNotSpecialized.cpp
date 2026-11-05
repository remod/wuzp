// wuzp-expect: error
// wuzp-static-assert: 'CastCoordinateSystem' trait not specialized for given types\.

// Using a trait with a type it is not specialized for fails with a clear message.

#include "wuzp/core/traits.hpp"

struct NotSupported {};

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::CastCoordinateSystem<NotSupported, NotSupported>);
  return 0;
}
