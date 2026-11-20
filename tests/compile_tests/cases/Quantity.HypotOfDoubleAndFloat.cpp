// wuzp-expect: error
// wuzp-static-assert: atan2, hypot, min and max need quantities with the same value type and value matrix backend\.

// hypot() takes two quantities with the same value type, like a sum, not a double and a float.

#include "wuzp/core/Quantities.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::hypot(QuantityD<Length>{1.0}, QuantityF<Length>{1.0F});
  return 0;
}
