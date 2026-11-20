// wuzp-expect: error
// wuzp-static-assert: atan2, hypot, min and max need quantities with the same unit\.

// atan2() takes two quantities with the same unit, not a length and a time.

#include "wuzp/core/Quantities.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::atan2(QuantityD<Length>{1.0}, QuantityD<Time>{1.0});
  return 0;
}
