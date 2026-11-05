// wuzp-expect: error
// wuzp-static-assert: 'Inverse' trait not specialized for given type\.

// Using a trait with a type it is not specialized for fails with a clear message.

#include "wuzp/core/traits.hpp"

struct NotSupported {};

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::Inverse<NotSupported>);
  return 0;
}
