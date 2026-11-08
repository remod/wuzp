// wuzp-expect: error
// wuzp-static-assert: Number of rows do not match\.

// The values of quantities can only be set to values of the same size (#62).

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  wuzp::Quantities<ValueMatrix<double, 3, 1>, wuzp::TypeMatrix<3, 1, Length>> quantities;
  quantities.setValues(ValueMatrix<double, 2, 1>{1.0, 2.0});
  return 0;
}
