// wuzp-expect: error
// wuzp-static-assert: sin, cos, tan, asin, acos, atan, exp and log need dimensionless quantities, e\.g\. angles or ratios of lengths\.

// The functions of dimensionless quantities apply elementwise, and reject quantities of which any element is not dimensionless,
// here the length in a vector of an angle and a length.

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  const wuzp::Quantities<ValueMatrix<double, 2, 1>, wuzp::TypeVector<2, Angle, Length>> quantities{0.5, 1.0};
  [[maybe_unused]] const auto result = wuzp::sin(quantities);
  return 0;
}
