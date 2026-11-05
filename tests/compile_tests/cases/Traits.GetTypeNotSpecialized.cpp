// wuzp-expect: error
// wuzp-static-assert: 'GetType' trait not specialized for given type\.

// Using a trait with a type it is not specialized for fails with a clear message.

#include "wuzp/core/traits.hpp"

struct NotSupported {};

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::GetType<NotSupported>);
  return 0;
}
