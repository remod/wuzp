// wuzp-expect: error
// wuzp-static-assert: 'Block' trait not specialized for given type\.

// Using a trait with a type it is not specialized for fails with a clear message.

#include "wuzp/core/traits.hpp"

struct NotSupported {};

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::Block<NotSupported, 0, 0, 1, 1>);
  return 0;
}
