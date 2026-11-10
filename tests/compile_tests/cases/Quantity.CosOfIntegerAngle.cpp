// wuzp-expect: error
// wuzp-static-assert: sin and cos need floating-point values\.

// The cosine of an angle with integer values is not an integer in general, and would be truncated.

#include "wuzp/core/Quantities.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::cos(QuantityI<Angle>{1});
  return 0;
}
