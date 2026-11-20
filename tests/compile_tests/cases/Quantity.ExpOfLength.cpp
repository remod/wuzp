// wuzp-expect: error
// wuzp-static-assert: sin, cos, tan, asin, acos, atan, exp and log need dimensionless quantities, e\.g\. angles or ratios of lengths\.

// The exponential function only takes dimensionless quantities, e.g. angles, not a length.

#include "wuzp/core/Quantities.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::exp(QuantityD<Length>{1.0});
  return 0;
}
