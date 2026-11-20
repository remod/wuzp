// wuzp-expect: error
// wuzp-static-assert: 'ValidityTolerance' trait not specialized for given types\.

// The tolerance of the validity checks of rotations and Lorentz transformations is only defined for arithmetic types, the value
// types of value matrices.

#include "wuzp/trafo/traits.hpp"

struct NotSupported {};

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::ValidityTolerance<NotSupported>);
  return 0;
}
