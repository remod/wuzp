// wuzp-expect: error
// wuzp-static-assert: sin, cos, tan, asin, acos, atan, atan2, exp and log need floating-point values: their results are not integers in general\.

// The angle of a point with integer coordinates is not an integer in general, and would be truncated.

#include "wuzp/core/Quantities.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::atan2(QuantityI<Length>{1}, QuantityI<Length>{1});
  return 0;
}
