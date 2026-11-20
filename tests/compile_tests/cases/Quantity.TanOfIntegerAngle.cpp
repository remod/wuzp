// wuzp-expect: error
// wuzp-static-assert: sin, cos, tan, asin, acos, atan, atan2, exp and log need floating-point values: their results are not integers in general\.

// The tangent of integer values is not an integer in general, and would be truncated.

#include "wuzp/core/Quantities.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::tan(QuantityI<Angle>{1});
  return 0;
}
